// ============================================================================
// core/route.c - Implementación de funciones de ruta
// ============================================================================
#include "route.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// CREACIÓN Y DESTRUCCIÓN DE RUTAS
// ============================================================================

Route* createRoute(int initial_capacity) {
    Route* route = (Route*)malloc(sizeof(Route));
    if (!route) {
        printf("❌ Error: No se pudo crear la ruta\n");
        return NULL;
    }

    // Inicializar capacidades
    route->capacity_path = initial_capacity;
    route->capacity_lines = initial_capacity;

    // Asignar memoria para arrays
    route->path = (Station**)malloc(route->capacity_path * sizeof(Station*));
    route->lines_used = (Line**)malloc(route->capacity_lines * sizeof(Line*));

    if (!route->path || !route->lines_used) {
        printf("❌ Error: No se pudo asignar memoria para arrays de ruta\n");
        free(route->path);
        free(route->lines_used);
        free(route);
        return NULL;
    }

    // Inicializar valores
    route->path_length = 0;
    route->transfer_count = 0;
    route->total_cost = 0.0;
    route->total_time_minutes = 0;
    route->departure_time = NULL;
    route->arrival_time = NULL;

    printf("✅ Ruta creada con capacidad para %d estaciones\n", initial_capacity);
    return route;
}

void destroyRoute(Route* route) {
    if (!route) return;

    // Liberar arrays
    free(route->path);
    free(route->lines_used);

    // Liberar tiempos si existen
    if (route->departure_time) {
        free(route->departure_time);
    }
    if (route->arrival_time) {
        free(route->arrival_time);
    }

    free(route);
    printf("🗑️ Ruta destruida\n");
}

// ============================================================================
// GESTIÓN DE ESTACIONES EN LA RUTA
// ============================================================================

bool addStationToRoute(Route* route, Station* station, Line* line) {
    if (!route || !station) {
        printf("❌ Error: Ruta o estación nula\n");
        return false;
    }

    // Verificar capacidad
    if (route->path_length >= route->capacity_path) {
        printf("❌ Error: Capacidad máxima de ruta alcanzada (%d)\n", route->capacity_path);
        return false;
    }

    // Agregar estación al camino
    route->path[route->path_length] = station;

    // Gestionar líneas y transbordos
    if (route->path_length == 0) {
        // Primera estación - no hay transbordo
        route->lines_used[route->path_length] = line;
    } else {
        // Verificar si cambia de línea (transbordo)
        Line* previous_line = route->lines_used[route->path_length - 1];

        if (line && previous_line && line != previous_line) {
            route->transfer_count++;
            printf("🔄 Transbordo detectado: %s → %s en %s\n",
                   previous_line->name, line->name, station->name);
        }

        route->lines_used[route->path_length] = line ? line : previous_line;
    }

    route->path_length++;

    printf("➕ Estación agregada a ruta: %s (Posición %d)\n",
           station->name, route->path_length);

    return true;
}

bool removeLastStationFromRoute(Route* route) {
    if (!route || route->path_length == 0) {
        printf("❌ Error: No hay estaciones para remover\n");
        return false;
    }

    Station* removed_station = route->path[route->path_length - 1];
    route->path_length--;

    // Recalcular transbordos si es necesario
    if (route->path_length > 1) {
        route->transfer_count = 0;
        for (int i = 1; i < route->path_length; i++) {
            if (route->lines_used[i] != route->lines_used[i - 1]) {
                route->transfer_count++;
            }
        }
    } else {
        route->transfer_count = 0;
    }

    printf("➖ Estación removida de ruta: %s\n", removed_station->name);
    return true;
}

bool insertStationAtPosition(Route* route, Station* station, Line* line, int position) {
    if (!route || !station || position < 0 || position > route->path_length) {
        printf("❌ Error: Parámetros inválidos para insertar estación\n");
        return false;
    }

    if (route->path_length >= route->capacity_path) {
        printf("❌ Error: Capacidad máxima alcanzada\n");
        return false;
    }

    // Mover elementos hacia la derecha
    for (int i = route->path_length; i > position; i--) {
        route->path[i] = route->path[i - 1];
        route->lines_used[i] = route->lines_used[i - 1];
    }

    // Insertar nueva estación
    route->path[position] = station;
    route->lines_used[position] = line;
    route->path_length++;

    // Recalcular transbordos
    route->transfer_count = 0;
    for (int i = 1; i < route->path_length; i++) {
        if (route->lines_used[i] && route->lines_used[i - 1] &&
            route->lines_used[i] != route->lines_used[i - 1]) {
            route->transfer_count++;
        }
    }

    printf("📍 Estación insertada en posición %d: %s\n", position, station->name);
    return true;
}

// ============================================================================
// GESTIÓN DE TIEMPOS
// ============================================================================

bool setDepartureTime(Route* route, DateTime departure) {
    if (!route) return false;

    if (!route->departure_time) {
        route->departure_time = (DateTime*)malloc(sizeof(DateTime));
        if (!route->departure_time) return false;
    }

    *route->departure_time = departure;

    printf("🕐 Hora de salida establecida: %02d:%02d\n",
           departure.time.hour, departure.time.minute);
    return true;
}

bool setArrivalTime(Route* route, DateTime arrival) {
    if (!route) return false;

    if (!route->arrival_time) {
        route->arrival_time = (DateTime*)malloc(sizeof(DateTime));
        if (!route->arrival_time) return false;
    }

    *route->arrival_time = arrival;

    printf("🏁 Hora de llegada establecida: %02d:%02d\n",
           arrival.time.hour, arrival.time.minute);
    return true;
}

bool calculateArrivalTime(Route* route) {
    if (!route || !route->departure_time || route->total_time_minutes <= 0) {
        printf("❌ Error: Datos insuficientes para calcular llegada\n");
        return false;
    }

    if (!route->arrival_time) {
        route->arrival_time = (DateTime*)malloc(sizeof(DateTime));
        if (!route->arrival_time) return false;
    }

    // Copiar fecha de salida
    route->arrival_time->date = route->departure_time->date;
    route->arrival_time->time = route->departure_time->time;

    // Agregar tiempo de viaje
    addMinutesToTime(&route->arrival_time->time, route->total_time_minutes);

    printf("⏰ Tiempo de llegada calculado: %02d:%02d (viaje de %d minutos)\n",
           route->arrival_time->time.hour, route->arrival_time->time.minute,
           route->total_time_minutes);

    return true;
}

// ============================================================================
// ANÁLISIS Y CÁLCULOS DE RUTA
// ============================================================================

int getRouteLength(Route* route) {
    return route ? route->path_length : 0;
}

double getRouteCost(Route* route) {
    return route ? route->total_cost : 0.0;
}

int getRouteTotalTime(Route* route) {
    return route ? route->total_time_minutes : 0;
}

int getRouteTransferCount(Route* route) {
    return route ? route->transfer_count : 0;
}

Station* getStationAtPosition(Route* route, int position) {
    if (!route || position < 0 || position >= route->path_length) {
        return NULL;
    }
    return route->path[position];
}

Line* getLineAtPosition(Route* route, int position) {
    if (!route || position < 0 || position >= route->path_length) {
        return NULL;
    }
    return route->lines_used[position];
}

bool isStationInRoute(Route* route, Station* station) {
    if (!route || !station) return false;

    for (int i = 0; i < route->path_length; i++) {
        if (route->path[i]->id == station->id) {
            return true;
        }
    }
    return false;
}

int getStationPositionInRoute(Route* route, Station* station) {
    if (!route || !station) return -1;

    for (int i = 0; i < route->path_length; i++) {
        if (route->path[i]->id == station->id) {
            return i;
        }
    }
    return -1;
}

// ============================================================================
// VALIDACIÓN DE RUTAS
// ============================================================================

bool isValidRoute(Route* route) {
    if (!route) {
        printf("❌ Ruta nula\n");
        return false;
    }

    if (route->path_length < 2) {
        printf("❌ Ruta debe tener al menos 2 estaciones\n");
        return false;
    }

    if (!route->path || !route->lines_used) {
        printf("❌ Arrays de ruta no inicializados\n");
        return false;
    }

    // Verificar que todas las estaciones existen
    for (int i = 0; i < route->path_length; i++) {
        if (!route->path[i]) {
            printf("❌ Estación nula en posición %d\n", i);
            return false;
        }
    }

    // Verificar coherencia de líneas
    for (int i = 0; i < route->path_length; i++) {
        if (!route->lines_used[i]) {
            printf("❌ Línea nula en posición %d\n", i);
            return false;
        }
    }

    printf("✅ Ruta válida: %d estaciones, %d transbordos\n",
           route->path_length, route->transfer_count);
    return true;
}

bool hasCircularPath(Route* route) {
    if (!route || route->path_length < 3) return false;

    Station* origin = route->path[0];
    Station* destination = route->path[route->path_length - 1];

    return (origin->id == destination->id);
}

// ============================================================================
// OPTIMIZACIÓN DE RUTAS
// ============================================================================

bool optimizeRoute(Route* route) {
    if (!isValidRoute(route)) return false;

    printf("🔧 Optimizando ruta...\n");

    // Eliminar estaciones duplicadas consecutivas
    bool optimized = false;
    for (int i = 1; i < route->path_length; i++) {
        if (route->path[i]->id == route->path[i - 1]->id) {
            // Remover duplicado
            for (int j = i; j < route->path_length - 1; j++) {
                route->path[j] = route->path[j + 1];
                route->lines_used[j] = route->lines_used[j + 1];
            }
            route->path_length--;
            optimized = true;
            i--; // Revisar la misma posición nuevamente
        }
    }

    // Recalcular transbordos después de optimización
    if (optimized) {
        route->transfer_count = 0;
        for (int i = 1; i < route->path_length; i++) {
            if (route->lines_used[i] != route->lines_used[i - 1]) {
                route->transfer_count++;
            }
        }
        printf("✅ Ruta optimizada: duplicados removidos\n");
    }

    return optimized;
}

bool reverseRoute(Route* route) {
    if (!route || route->path_length < 2) return false;

    printf("🔄 Invirtiendo ruta...\n");

    // Invertir arrays
    for (int i = 0; i < route->path_length / 2; i++) {
        int opposite = route->path_length - 1 - i;

        // Intercambiar estaciones
        Station* temp_station = route->path[i];
        route->path[i] = route->path[opposite];
        route->path[opposite] = temp_station;

        // Intercambiar líneas
        Line* temp_line = route->lines_used[i];
        route->lines_used[i] = route->lines_used[opposite];
        route->lines_used[opposite] = temp_line;
    }

    // Intercambiar tiempos de salida y llegada
    if (route->departure_time && route->arrival_time) {
        DateTime temp_time = *route->departure_time;
        *route->departure_time = *route->arrival_time;
        *route->arrival_time = temp_time;
    }

    printf("✅ Ruta invertida exitosamente\n");
    return true;
}

// ============================================================================
// VISUALIZACIÓN DE RUTAS
// ============================================================================

void printRoute(Route* route) {
    if (!route || route->path_length == 0) {
        printf("❌ Ruta vacía o inválida\n");
        return;
    }

    printf("\n🗺️  ===== RUTA CALCULADA =====\n");
    printf("📍 Origen: %s\n", route->path[0]->name);
    printf("🎯 Destino: %s\n", route->path[route->path_length - 1]->name);
    printf("💰 Costo total: $%.2f\n", route->total_cost);
    printf("⏱️  Tiempo total: %d minutos\n", route->total_time_minutes);
    printf("🔄 Transbordos: %d\n", route->transfer_count);

    if (route->departure_time) {
        printf("🕐 Salida: %02d:%02d\n",
               route->departure_time->time.hour, route->departure_time->time.minute);
    }

    if (route->arrival_time) {
        printf("🏁 Llegada: %02d:%02d\n",
               route->arrival_time->time.hour, route->arrival_time->time.minute);
    }

    printf("\n📋 Recorrido:\n");
    Line* current_line = NULL;

    for (int i = 0; i < route->path_length; i++) {
        Station* station = route->path[i];
        Line* line = route->lines_used[i];

        if (current_line != line) {
            if (current_line != NULL) {
                printf("   🔄 TRANSBORDO\n");
            }
            printf("   🚇 Línea: %s (%s)\n", line->name, line->color);
            current_line = line;
        }

        printf("   %d. 🚉 %s", i + 1, station->name);

        if (i < route->path_length - 1) {
            // Buscar tiempo de viaje si está disponible
            for (int j = 0; j < line->station_count - 1; j++) {
                if (line->stations[j]->id == station->id) {
                    printf(" → %d min", line->travel_times[j]);
                    break;
                }
            }
        }
        printf("\n");
    }
    printf("=============================\n\n");
}

void printRouteCompact(Route* route) {
    if (!route || route->path_length == 0) {
        printf("❌ Ruta vacía\n");
        return;
    }

    printf("🗺️ %s", route->path[0]->name);
    for (int i = 1; i < route->path_length; i++) {
        printf(" → %s", route->path[i]->name);
    }
    printf(" (%d min, $%.2f, %d transbordos)\n",
           route->total_time_minutes, route->total_cost, route->transfer_count);
}

// ============================================================================
// COMPARACIÓN DE RUTAS
// ============================================================================

int compareRoutesByTime(const void* a, const void* b) {
    Route* route_a = *(Route**)a;
    Route* route_b = *(Route**)b;

    return route_a->total_time_minutes - route_b->total_time_minutes;
}

int compareRoutesByCost(const void* a, const void* b) {
    Route* route_a = *(Route**)a;
    Route* route_b = *(Route**)b;

    if (route_a->total_cost < route_b->total_cost) return -1;
    if (route_a->total_cost > route_b->total_cost) return 1;
    return 0;
}

int compareRoutesByTransfers(const void* a, const void* b) {
    Route* route_a = *(Route**)a;
    Route* route_b = *(Route**)b;

    return route_a->transfer_count - route_b->transfer_count;
}

// ============================================================================
// UTILIDADES DE COPIA Y CLONACIÓN
// ============================================================================

Route* cloneRoute(Route* original) {
    if (!original) return NULL;

    Route* clone = createRoute(original->capacity_path);
    if (!clone) return NULL;

    // Copiar todas las estaciones y líneas
    for (int i = 0; i < original->path_length; i++) {
        addStationToRoute(clone, original->path[i], original->lines_used[i]);
    }

    // Copiar propiedades
    clone->total_cost = original->total_cost;
    clone->total_time_minutes = original->total_time_minutes;
    clone->transfer_count = original->transfer_count;

    // Copiar tiempos si existen
    if (original->departure_time) {
        setDepartureTime(clone, *original->departure_time);
    }
    if (original->arrival_time) {
        setArrivalTime(clone, *original->arrival_time);
    }

    printf("📋 Ruta clonada exitosamente\n");
    return clone;
}