#include "gps_system.h"
#include "../algoritmos/bellman_ford.h"
#include "../algoritmos/dfs_bfs.h"
#include "../algoritmos/dijkstra.h"
#include "../estructura_datos/hash_map.h"
#include "../estructura_datos/priority_queue.h"
#include "../algoritmos/cycle_detection.h"

#include "../graph/graph.h"

// Crear sistema de navegación
NavigationSystem* createNavigationSystem(int maxCities) {
    NavigationSystem* gps = (NavigationSystem*)malloc(sizeof(NavigationSystem));
    if (!gps) return NULL;
    
    // Inicializar red de carreteras
    gps->network = (RoadNetwork*)malloc(sizeof(RoadNetwork));
    gps->network->cities = (City*)calloc(maxCities, sizeof(City));
    gps->network->roads = (Road*)calloc(maxCities * maxCities, sizeof(Road));
    gps->network->numCities = 0;
    gps->network->numRoads = 0;
    gps->network->capacity = maxCities;
    
    // Inicializar matriz de adyacencia
    gps->network->adjacencyMatrix = (int**)malloc(maxCities * sizeof(int*));
    for (int i = 0; i < maxCities; i++) {
        gps->network->adjacencyMatrix[i] = (int*)calloc(maxCities, sizeof(int));
    }
    
    // Inicializar HashMap para búsqueda de ciudades
    gps->cityIndex = createHashMap(maxCities * 2);
    
    // Inicializar cola de prioridad para tráfico
    gps->trafficQueue = createPriorityQueue(100, true); // Min-heap
    
    // Inicializar caché de rutas
    gps->routeCache = (RouteCache*)malloc(sizeof(RouteCache));
    gps->routeCache->capacity = CACHE_SIZE;
    gps->routeCache->buckets = (CacheEntry**)calloc(CACHE_SIZE, sizeof(CacheEntry*));
    gps->routeCache->size = 0;
    
    gps->lastMaintenanceTime = time(NULL);
    gps->debugMode = false;
    
    printf("✅ Sistema de navegación GPS creado exitosamente\n");
    printf("   Capacidad máxima: %d ciudades\n", maxCities);
    
    return gps;
}

// Agregar ciudad al sistema
int addCity(NavigationSystem* gps, const char* name, double lat, double lon, int population, const char* region) {
    if (!gps || gps->network->numCities >= gps->network->capacity) {
        printf("❌ Error: No se puede agregar más ciudades\n");
        return -1;
    }
    
    // Verificar si la ciudad ya existe
    if (hashMapGet(gps->cityIndex, name)) {
        printf("⚠️  Ciudad '%s' ya existe en el sistema\n", name);
        return -1;
    }
    
    int cityId = gps->network->numCities;
    City* city = &gps->network->cities[cityId];
    
    // Llenar datos de la ciudad
    city->id = cityId;
    strncpy(city->name, name, MAX_NAME_LENGTH - 1);
    city->location.latitude = lat;
    city->location.longitude = lon;
    city->population = population;
    strncpy(city->region, region, MAX_NAME_LENGTH - 1);
    city->isActive = true;
    
    // Agregar al HashMap
    hashMapPut(gps->cityIndex, name, (void*)(intptr_t)cityId);
    
    gps->network->numCities++;
    
    if (gps->debugMode) {
        printf("🏙️  Ciudad agregada: %s (ID: %d, Población: %d, Región: %s)\n", 
               name, cityId, population, region);
    }
    
    return cityId;
}

// Buscar ciudad por nombre
City* findCity(NavigationSystem* gps, const char* name) {
    if (!gps || !name) return NULL;
    
    void* result = hashMapGet(gps->cityIndex, name);
    if (!result) return NULL;
    
    int cityId = (intptr_t)result;
    if (cityId >= 0 && cityId < gps->network->numCities) {
        return &gps->network->cities[cityId];
    }
    
    return NULL;
}

// Agregar carretera
bool addRoad(NavigationSystem* gps, const char* fromCity, const char* toCity, 
             double distance, double travelTime, double toll, const char* roadType, int speedLimit) {
    City* from = findCity(gps, fromCity);
    City* to = findCity(gps, toCity);

    if (!from || !to) {
        printf("❌ Error: Una o ambas ciudades no encontradas (%s, %s)\n", fromCity, toCity);
        return false;
    }

    if (from->id == to->id) {
        printf("❌ Error: No se pueden crear carreteras de una ciudad a sí misma\n");
        return false;
    }

    // Crear carretera
    Road* road = &gps->network->roads[gps->network->numRoads];
    road->from = from->id;
    road->to = to->id;
    road->distance = distance;
    road->baseTime = travelTime;
    road->currentTime = travelTime;
    road->toll = toll;
    road->speedLimit = speedLimit;
    strncpy(road->roadType, roadType, 19);
    road->isClosed = false;
    road->lastUpdate = time(NULL);

    // Actualizar matriz de adyacencia (grafo no dirigido)
    gps->network->adjacencyMatrix[from->id][to->id] = (int)travelTime;
    gps->network->adjacencyMatrix[to->id][from->id] = (int)travelTime;

    gps->network->numRoads++;

    // Invalidar caché afectado
    clearRouteCache(gps);

    if (gps->debugMode) {
        printf("🛣️  Carretera agregada: %s ↔ %s (%.1f km, %.0f min, $%.2f, %s)\n",
               fromCity, toCity, distance, travelTime, toll, roadType);
    }

    return true;
}

// Calcular distancia Haversine entre dos puntos
double calculateHaversineDistance(GeoCoordinate p1, GeoCoordinate p2) {
    const double R = 6371.0; // Radio de la Tierra en km

    double lat1Rad = p1.latitude * M_PI / 180.0;
    double lat2Rad = p2.latitude * M_PI / 180.0;
    double deltaLat = (p2.latitude - p1.latitude) * M_PI / 180.0;
    double deltaLon = (p2.longitude - p1.longitude) * M_PI / 180.0;

    double a = sin(deltaLat/2) * sin(deltaLat/2) +
               cos(lat1Rad) * cos(lat2Rad) * sin(deltaLon/2) * sin(deltaLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return R * c;
}

// Implementar algoritmos de búsqueda usando las funciones existentes
Route* findShortestPath(NavigationSystem* gps, const char* from, const char* to) {
    City* fromCity = findCity(gps, from);
    City* toCity = findCity(gps, to);

    if (!fromCity || !toCity) {
        printf("❌ Error: Ciudad no encontrada\n");
        return NULL;
    }

    // Verificar caché primero
    Route* cached = getCachedRoute(gps, from, to, "shortest");
    if (cached) {
        if (gps->debugMode) printf("🔄 Ruta obtenida del caché\n");
        return cached;
    }

    // Usar Dijkstra para encontrar la ruta más corta
    PathResult* result = dijkstra(gps->network->adjacencyMatrix, gps->network->numCities,
                                  fromCity->id, toCity->id);

    if (!result || !result->hasPath) {
        printf("❌ No existe ruta entre %s y %s\n", from, to);
        if (result) freePathResult(result);
        return NULL;
    }

    // Crear estructura Route
    Route* route = (Route*)malloc(sizeof(Route));
    route->cityPath = (int*)malloc(result->pathLength * sizeof(int));
    memcpy(route->cityPath, result->path, result->pathLength * sizeof(int));
    route->pathLength = result->pathLength;
    route->totalTime = result->totalWeight;
    strcpy(route->routeType, "shortest");
    route->calculatedTime = time(NULL);
    route->isValid = true;

    // Calcular distancia y costo real recorriendo el camino
    route->totalDistance = 0;
    route->totalCost = 0;

    for (int i = 0; i < route->pathLength - 1; i++) {
        int fromId = route->cityPath[i];
        int toId = route->cityPath[i + 1];

        // Buscar la carretera correspondiente
        for (int r = 0; r < gps->network->numRoads; r++) {
            Road* road = &gps->network->roads[r];
            if ((road->from == fromId && road->to == toId) ||
                (road->from == toId && road->to == fromId)) {
                route->totalDistance += road->distance;
                route->totalCost += road->toll;
                break;
            }
        }
    }

    // Cachear la ruta
    cacheRoute(gps, from, to, route);

    freePathResult(result);
    return route;
}

// Ruta más rápida (considera tráfico actual)
Route* findFastestPath(NavigationSystem* gps, const char* from, const char* to) {
    City* fromCity = findCity(gps, from);
    City* toCity = findCity(gps, to);

    if (!fromCity || !toCity) return NULL;

    // Crear matriz temporal con tiempos actuales (incluyendo tráfico)
    int** timeMatrix = (int**)malloc(gps->network->numCities * sizeof(int*));
    for (int i = 0; i < gps->network->numCities; i++) {
        timeMatrix[i] = (int*)calloc(gps->network->numCities, sizeof(int));
    }

    // Llenar matriz con tiempos actuales
    for (int r = 0; r < gps->network->numRoads; r++) {
        Road* road = &gps->network->roads[r];
        if (!road->isClosed) {
            timeMatrix[road->from][road->to] = (int)road->currentTime;
            timeMatrix[road->to][road->from] = (int)road->currentTime;
        }
    }

    PathResult* result = dijkstra(timeMatrix, gps->network->numCities, fromCity->id, toCity->id);

    Route* route = NULL;
    if (result && result->hasPath) {
        route = (Route*)malloc(sizeof(Route));
        route->cityPath = (int*)malloc(result->pathLength * sizeof(int));
        memcpy(route->cityPath, result->path, result->pathLength * sizeof(int));
        route->pathLength = result->pathLength;
        route->totalTime = result->totalWeight;
        strcpy(route->routeType, "fastest");
        route->calculatedTime = time(NULL);
        route->isValid = true;

        // Calcular distancia y costo
        route->totalDistance = 0;
        route->totalCost = 0;
        for (int i = 0; i < route->pathLength - 1; i++) {
            int fromId = route->cityPath[i];
            int toId = route->cityPath[i + 1];

            for (int r = 0; r < gps->network->numRoads; r++) {
                Road* road = &gps->network->roads[r];
                if ((road->from == fromId && road->to == toId) ||
                    (road->from == toId && road->to == fromId)) {
                    route->totalDistance += road->distance;
                    route->totalCost += road->toll;
                    break;
                }
            }
        }
    }

    // Limpiar matriz temporal
    for (int i = 0; i < gps->network->numCities; i++) {
        free(timeMatrix[i]);
    }
    free(timeMatrix);

    if (result) freePathResult(result);
    return route;
}

// Ruta más económica (considera peajes y descuentos)
Route* findCheapestPath(NavigationSystem* gps, const char* from, const char* to) {
    City* fromCity = findCity(gps, from);
    City* toCity = findCity(gps, to);

    if (!fromCity || !toCity) return NULL;

    // Crear matriz con costos (usar Bellman-Ford para manejar peajes negativos)
    int** costMatrix = (int**)malloc(gps->network->numCities * sizeof(int*));
    for (int i = 0; i < gps->network->numCities; i++) {
        costMatrix[i] = (int*)calloc(gps->network->numCities, sizeof(int));
    }

    // Llenar matriz con costos (multiplicar por 100 para evitar decimales)
    for (int r = 0; r < gps->network->numRoads; r++) {
        Road* road = &gps->network->roads[r];
        if (!road->isClosed) {
            int cost = (int)(road->toll * 100); // Convertir a centavos
            costMatrix[road->from][road->to] = cost;
            costMatrix[road->to][road->from] = cost;
        }
    }

    PathResult* result = bellmanFord(costMatrix, gps->network->numCities, fromCity->id, toCity->id);

    Route* route = NULL;
    if (result && result->hasPath) {
        route = (Route*)malloc(sizeof(Route));
        route->cityPath = (int*)malloc(result->pathLength * sizeof(int));
        memcpy(route->cityPath, result->path, result->pathLength * sizeof(int));
        route->pathLength = result->pathLength;
        route->totalCost = result->totalWeight / 100.0; // Convertir de centavos
        strcpy(route->routeType, "cheapest");
        route->calculatedTime = time(NULL);
        route->isValid = true;

        // Calcular distancia y tiempo
        route->totalDistance = 0;
        route->totalTime = 0;
        for (int i = 0; i < route->pathLength - 1; i++) {
            int fromId = route->cityPath[i];
            int toId = route->cityPath[i + 1];

            for (int r = 0; r < gps->network->numRoads; r++) {
                Road* road = &gps->network->roads[r];
                if ((road->from == fromId && road->to == toId) ||
                    (road->from == toId && road->to == fromId)) {
                    route->totalDistance += road->distance;
                    route->totalTime += road->currentTime;
                    break;
                }
            }
        }
    }

    // Limpiar matriz temporal
    for (int i = 0; i < gps->network->numCities; i++) {
        free(costMatrix[i]);
    }
    free(costMatrix);

    if (result) freePathResult(result);
    return route;
}

// Verificar conectividad usando BFS
bool isReachable(NavigationSystem* gps, const char* from, const char* to) {
    City* fromCity = findCity(gps, from);
    City* toCity = findCity(gps, to);

    if (!fromCity || !toCity) return false;
    if (fromCity->id == toCity->id) return true;

    return isConnected(gps->network->adjacencyMatrix, gps->network->numCities,
                      fromCity->id, toCity->id);
}

// Actualizar condiciones de tráfico
void updateTrafficConditions(NavigationSystem* gps, const char* fromCity, const char* toCity, double trafficFactor) {
    City* from = findCity(gps, fromCity);
    City* to = findCity(gps, toCity);

    if (!from || !to) {
        printf("❌ Error: Ciudad no encontrada para actualización de tráfico\n");
        return;
    }

    // Buscar y actualizar la carretera
    bool roadFound = false;
    for (int r = 0; r < gps->network->numRoads; r++) {
        Road* road = &gps->network->roads[r];
        if ((road->from == from->id && road->to == to->id) ||
            (road->from == to->id && road->to == from->id)) {
            road->currentTime = road->baseTime * trafficFactor;
            road->lastUpdate = time(NULL);
            roadFound = true;

            // Actualizar matriz de adyacencia
            gps->network->adjacencyMatrix[road->from][road->to] = (int)road->currentTime;
            gps->network->adjacencyMatrix[road->to][road->from] = (int)road->currentTime;

            break;
        }
    }

    if (roadFound) {
        // Invalidar caché de rutas afectadas
        clearRouteCache(gps);

        if (gps->debugMode) {
            printf("🚦 Tráfico actualizado: %s ↔ %s (factor: %.2f)\n",
                   fromCity, toCity, trafficFactor);
        }
    } else {
        printf("❌ Error: Carretera no encontrada entre %s y %s\n", fromCity, toCity);
    }
}

// Detectar ciclos en la red (loops de rutas)
bool detectRouteLoops(NavigationSystem* gps) {
    if (!gps || gps->network->numCities == 0) return false;

    // Usar función de detección de ciclos para grafo no dirigido
    bool hasCycles = hasCycleUndirected(gps->network->adjacencyMatrix, gps->network->numCities);

    if (gps->debugMode) {
        printf("🔄 Detección de ciclos: %s\n", hasCycles ? "Se encontraron ciclos" : "No hay ciclos");
    }

    return hasCycles;
}

// Hash para caché de rutas
unsigned int hashRouteKey(const char* from, const char* to, const char* type) {
    unsigned int hash = 5381;

    // Hash del string concatenado: from+to+type
    char combined[200];
    snprintf(combined, sizeof(combined), "%s->%s:%s", from, to, type);

    for (int i = 0; combined[i]; i++) {
        hash = ((hash << 5) + hash) + combined[i];
    }

    return hash;
}

// Obtener ruta del caché
Route* getCachedRoute(NavigationSystem* gps, const char* from, const char* to, const char* routeType) {
    if (!gps || !gps->routeCache) return NULL;

    unsigned int index = hashRouteKey(from, to, routeType) % gps->routeCache->capacity;
    CacheEntry* entry = gps->routeCache->buckets[index];

    time_t currentTime = time(NULL);

    while (entry) {
        City* fromCity = findCity(gps, from);
        City* toCity = findCity(gps, to);

        if (fromCity && toCity &&
            entry->fromCity == fromCity->id &&
            entry->toCity == toCity->id &&
            strcmp(entry->route->routeType, routeType) == 0) {

            // Verificar si la ruta no ha expirado (5 minutos)
            if (currentTime - entry->timestamp < 300) {
                return entry->route;
            } else {
                // Marcar como inválida
                entry->route->isValid = false;
            }
        }
        entry = entry->next;
    }

    return NULL;
}

// Cachear ruta
void cacheRoute(NavigationSystem* gps, const char* from, const char* to, Route* route) {
    if (!gps || !gps->routeCache || !route) return;

    City* fromCity = findCity(gps, from);
    City* toCity = findCity(gps, to);
    if (!fromCity || !toCity) return;

    unsigned int index = hashRouteKey(from, to, route->routeType) % gps->routeCache->capacity;

    CacheEntry* newEntry = (CacheEntry*)malloc(sizeof(CacheEntry));
    newEntry->fromCity = fromCity->id;
    newEntry->toCity = toCity->id;
    newEntry->route = route;
    newEntry->timestamp = time(NULL);
    newEntry->next = gps->routeCache->buckets[index];

    gps->routeCache->buckets[index] = newEntry;
    gps->routeCache->size++;

    if (gps->debugMode) {
        printf("💾 Ruta cacheada: %s → %s (%s)\n", from, to, route->routeType);
    }
}

// Limpiar caché de rutas
void clearRouteCache(NavigationSystem* gps) {
    if (!gps || !gps->routeCache) return;

    for (int i = 0; i < gps->routeCache->capacity; i++) {
        CacheEntry* entry = gps->routeCache->buckets[i];
        while (entry) {
            CacheEntry* next = entry->next;
            freeRoute(entry->route);
            free(entry);
            entry = next;
        }
        gps->routeCache->buckets[i] = NULL;
    }

    gps->routeCache->size = 0;

    if (gps->debugMode) {
        printf("🗑️  Caché de rutas limpiado\n");
    }
}

// Imprimir ruta
void printRoute(NavigationSystem* gps, Route* route) {
    if (!gps || !route || !route->isValid) {
        printf("❌ Ruta no válida\n");
        return;
    }

    printf("\n🗺️  === RUTA %s ===\n", route->routeType);
    printf("📍 Camino: ");

    for (int i = 0; i < route->pathLength; i++) {
        City* city = &gps->network->cities[route->cityPath[i]];
        printf("%s", city->name);
        if (i < route->pathLength - 1) printf(" → ");
    }

    printf("\n📏 Distancia total: %.1f km\n", route->totalDistance);
    printf("⏱️  Tiempo total: %.1f minutos (%.1f horas)\n", route->totalTime, route->totalTime / 60.0);
    printf("💰 Costo total: $%.2f\n", route->totalCost);
    printf("🔢 Paradas: %d ciudades\n", route->pathLength);

    time_t now = time(NULL);
    int age = (int)(now - route->calculatedTime);
    printf("📅 Calculado hace: %d segundos\n", age);

    printf("===============================\n");
}

// Liberar memoria de ruta
void freeRoute(Route* route) {
    if (route) {
        if (route->cityPath) free(route->cityPath);
        free(route);
    }
}

// Generar estadísticas de la red
void generateNetworkStatistics(NavigationSystem* gps) {
    if (!gps) return;

    printf("\n📊 === ESTADÍSTICAS DE LA RED ===\n");
    printf("🏙️  Ciudades registradas: %d\n", gps->network->numCities);
    printf("🛣️  Carreteras activas: %d\n", gps->network->numRoads);

    // Calcular estadísticas de carreteras
    double totalDistance = 0, totalTime = 0, totalCost = 0;
    int closedRoads = 0;

    for (int r = 0; r < gps->network->numRoads; r++) {
        Road* road = &gps->network->roads[r];
        totalDistance += road->distance;
        totalTime += road->currentTime;
        totalCost += road->toll;
        if (road->isClosed) closedRoads++;
    }

    if (gps->network->numRoads > 0) {
        printf("📏 Distancia total de red: %.1f km\n", totalDistance);
        printf("⏱️  Tiempo promedio por carretera: %.1f min\n", totalTime / gps->network->numRoads);
        printf("💰 Peaje promedio: $%.2f\n", totalCost / gps->network->numRoads);
        printf("🚫 Carreteras cerradas: %d (%.1f%%)\n", closedRoads,
               (double)closedRoads / gps->network->numRoads * 100);
    }

    // Analizar conectividad
    bool isConnected = isGraphConnected(gps->network->adjacencyMatrix, gps->network->numCities);
    printf("🔗 Red conectada: %s\n", isConnected ? "SÍ" : "NO");

    // Componentes conexos
    int numComponents;
    int* components = getConnectedComponents(gps->network->adjacencyMatrix,
                                           gps->network->numCities, &numComponents);
    printf("🌐 Componentes conexos: %d\n", numComponents);

    // Estadísticas del caché
    printf("💾 Rutas en caché: %d/%d\n", gps->routeCache->size, gps->routeCache->capacity);

    // Detectar ciclos
    bool hasCycles = detectRouteLoops(gps);
    printf("🔄 Ciclos detectados: %s\n", hasCycles ? "SÍ" : "NO");

    free(components);
    printf("=================================\n");
}

// Listar ciudades
void listCities(NavigationSystem* gps) {
    if (!gps || gps->network->numCities == 0) {
        printf("📍 No hay ciudades registradas\n");
        return;
    }

    printf("\n🏙️  === CIUDADES REGISTRADAS ===\n");
    printf("%-3s %-20s %-12s %-12s %-10s %-15s\n",
           "ID", "Nombre", "Latitud", "Longitud", "Población", "Región");
    printf("%-3s %-20s %-12s %-12s %-10s %-15s\n",
           "---", "--------------------", "------------", "------------", "----------", "---------------");

    for (int i = 0; i < gps->network->numCities; i++) {
        City* city = &gps->network->cities[i];
        if (city->isActive) {
            printf("%-3d %-20s %-12.4f %-12.4f %-10d %-15s\n",
                   city->id, city->name, city->location.latitude, city->location.longitude,
                   city->population, city->region);
        }
    }
    printf("================================\n");
}

// Destruir sistema de navegación
void destroyNavigationSystem(NavigationSystem* gps) {
    if (!gps) return;

    // Limpiar red de carreteras
    if (gps->network) {
        if (gps->network->adjacencyMatrix) {
            for (int i = 0; i < gps->network->capacity; i++) {
                free(gps->network->adjacencyMatrix[i]);
            }
            free(gps->network->adjacencyMatrix);
        }

        if (gps->network->cities) free(gps->network->cities);
        if (gps->network->roads) free(gps->network->roads);
        free(gps->network);
    }

    // Limpiar HashMap
    if (gps->cityIndex) destroyHashMap(gps->cityIndex);

    // Limpiar cola de prioridad
    if (gps->trafficQueue) destroyPriorityQueue(gps->trafficQueue);

    // Limpiar caché
    if (gps->routeCache) {
        clearRouteCache(gps);
        free(gps->routeCache->buckets);
        free(gps->routeCache);
    }

    free(gps);
    printf("🗑️  Sistema GPS destruido\n");
}

// =================================================================
// main_gps.c - Programa principal del GPS
// =================================================================

void showMenu() {
    printf("\n🗺️  === SISTEMA DE NAVEGACIÓN GPS ===\n");
    printf("1.  🏙️  Agregar ciudad\n");
    printf("2.  🛣️  Agregar carretera\n");
    printf("3.  📍 Listar ciudades\n");
    printf("4.  🎯 Buscar ruta más corta\n");
    printf("5.  ⚡ Buscar ruta más rápida\n");
    printf("6.  💰 Buscar ruta más económica\n");
    printf("7.  🔗 Verificar conectividad\n");
    printf("8.  🚦 Actualizar tráfico\n");
    printf("9.  🔄 Detectar ciclos\n");
    printf("10. 📊 Estadísticas de red\n");
    printf("11. 💾 Guardar sistema\n");
    printf("12. 📂 Cargar sistema\n");
    printf("13. 🗑️  Limpiar caché\n");
    printf("14. 🎨 Exportar a SVG\n");
    printf("15. 🐛 Toggle debug mode\n");
    printf("0.  🚪 Salir\n");
    printf("=====================================\n");
    printf("Seleccione una opción: ");
}

void runGPSDemo() {
    printf("🚀 DEMO DEL SISTEMA GPS\n");
    printf("=======================\n");

    NavigationSystem* gps = createNavigationSystem(50);
    gps->debugMode = true;

    // Agregar ciudades argentinas
    printf("\n📍 Agregando ciudades...\n");
    addCity(gps, "Buenos Aires", -34.6037, -58.3816, 3000000, "CABA");
    addCity(gps, "Córdoba", -31.4201, -64.1888, 1500000, "Córdoba");
    addCity(gps, "Rosario", -32.9442, -60.6505, 1200000, "Santa Fe");
    addCity(gps, "Mendoza", -32.8895, -68.8458, 120000, "Mendoza");
    addCity(gps, "La Plata", -34.9215, -57.9545, 800000, "Buenos Aires");
    addCity(gps, "Mar del Plata", -38.0055, -57.5426, 650000, "Buenos Aires");
    addCity(gps, "Salta", -24.7821, -65.4232, 530000, "Salta");
    addCity(gps, "Santa Fe", -31.6333, -60.7000, 400000, "Santa Fe");

    // Agregar carreteras
    printf("\n🛣️  Construyendo red de carreteras...\n");
    addRoad(gps, "Buenos Aires", "La Plata", 56, 45, 15.0, "highway", 120);
    addRoad(gps, "Buenos Aires", "Rosario", 300, 180, 25.0, "highway", 130);
    addRoad(gps, "Buenos Aires", "Mar del Plata", 404, 240, 30.0, "highway", 120);
    addRoad(gps, "Buenos Aires", "Córdoba", 695, 420, 50.0, "highway", 130);
    addRoad(gps, "Rosario", "Córdoba", 210, 135, 20.0, "highway", 120);
    addRoad(gps, "Rosario", "Santa Fe", 157, 105, 18.0, "highway", 110);
    addRoad(gps, "Córdoba", "Mendoza", 600, 360, 45.0, "highway", 120);
    addRoad(gps, "Córdoba", "Salta", 570, 380, 40.0, "highway", 110);
    addRoad(gps, "Santa Fe", "Salta", 690, 450, 48.0, "highway", 100);

    // Carretera escénica con descuento
    addRoad(gps, "La Plata", "Mar del Plata", 350, 210, -10.0, "scenic", 90);

    // Mostrar estadísticas iniciales
    generateNetworkStatistics(gps);

    // Demostrar búsqueda de rutas
    printf("\n🎯 === DEMOSTRANDO BÚSQUEDA DE RUTAS ===\n");

    Route* shortest = findShortestPath(gps, "Buenos Aires", "Salta");
    if (shortest) {
        printRoute(gps, shortest);
    }

    Route* fastest = findFastestPath(gps, "Buenos Aires", "Salta");
    if (fastest) {
        printRoute(gps, fastest);
    }

    Route* cheapest = findCheapestPath(gps, "Buenos Aires", "Salta");
    if (cheapest) {
        printRoute(gps, cheapest);
    }

    // Simular tráfico
    printf("\n🚦 Simulando congestión de tráfico...\n");
    updateTrafficConditions(gps, "Buenos Aires", "Córdoba", 1.8); // 80% más tiempo
    updateTrafficConditions(gps, "Rosario", "Córdoba", 1.5); // 50% más tiempo

    printf("\n🔄 Recalculando ruta más rápida con tráfico...\n");
    Route* fastestWithTraffic = findFastestPath(gps, "Buenos Aires", "Salta");
    if (fastestWithTraffic) {
        printRoute(gps, fastestWithTraffic);
    }

    // Verificar conectividad
    printf("\n🔗 === ANÁLISIS DE CONECTIVIDAD ===\n");
    printf("¿Buenos Aires → Salta? %s\n", isReachable(gps, "Buenos Aires", "Salta") ? "✅ SÍ" : "❌ NO");
    printf("¿Hay ciclos en la red? %s\n", detectRouteLoops(gps) ? "✅ SÍ" : "❌ NO");

    // Estadísticas finales
    generateNetworkStatistics(gps);

    // Limpiar memoria
    if (shortest) freeRoute(shortest);
    if (fastest) freeRoute(fastest);
    if (cheapest) freeRoute(cheapest);
    if (fastestWithTraffic) freeRoute(fastestWithTraffic);

    destroyNavigationSystem(gps);
}


