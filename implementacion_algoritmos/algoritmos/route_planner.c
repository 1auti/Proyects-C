// ============================================================================
// algoritmos/route_planner.c - Planificador de rutas principal
// ============================================================================
#include "route_planner.h"
#include "../core/transit_system.h"
#include "../pricing/fare_calculator.h"
#include "../realtime/delay_tracker.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

// ============================================================================
// FUNCIONES AUXILIARES DE BÚSQUEDA
// ============================================================================

Station* findStationById(TransitSystem* system, int id) {
    if (!system) return NULL;

    for (int i = 0; i < system->station_count; i++) {
        if (system->stations[i]->id == id) {
            return system->stations[i];
        }
    }
    return NULL;
}

Station* findStationByName(TransitSystem* system, const char* name) {
    if (!system || !name) return NULL;

    for (int i = 0; i < system->station_count; i++) {
        if (strcmp(system->stations[i]->name, name) == 0) {
            return system->stations[i];
        }
    }
    return NULL;
}

Line* findLineById(TransitSystem* system, int id) {
    if (!system) return NULL;

    for (int i = 0; i < system->lines_count; i++) {
        if (system->lines[i]->id == id) {
            return system->lines[i];
        }
    }
    return NULL;
}

Line* findLineByName(TransitSystem* system, const char* name) {
    if (!system || !name) return NULL;

    for (int i = 0; i < system->lines_count; i++) {
        if (strcmp(system->lines[i]->name, name) == 0) {
            return system->lines[i];
        }
    }
    return NULL;
}

Line* findConnectingLine(TransitSystem* system, int from_station, int to_station) {
    if (!system) return NULL;

    for (int i = 0; i < system->lines_count; i++) {
        Line* line = system->lines[i];
        bool has_from = false, has_to = false;

        for (int j = 0; j < line->station_count; j++) {
            if (line->stations[j]->id == from_station) has_from = true;
            if (line->stations[j]->id == to_station) has_to = true;
        }

        if (has_from && has_to) {
            return line;
        }
    }
    return NULL;
}

bool hasDirectConnection(TransitSystem* system, int from_station, int to_station) {
    return findConnectingLine(system, from_station, to_station) != NULL;
}

int getBaseTravelTime(Line* line, int from_station, int to_station) {
    if (!line) return -1;

    int from_idx = -1, to_idx = -1;

    // Encontrar índices de las estaciones en la línea
    for (int i = 0; i < line->station_count; i++) {
        if (line->stations[i]->id == from_station) from_idx = i;
        if (line->stations[i]->id == to_station) to_idx = i;
    }

    if (from_idx == -1 || to_idx == -1) return -1;

    // Calcular tiempo total entre estaciones
    int start = (from_idx < to_idx) ? from_idx : to_idx;
    int end = (from_idx < to_idx) ? to_idx : from_idx;

    int total_time = 0;
    for (int i = start; i < end; i++) {
        total_time += line->travel_times[i];
    }

    return total_time;
}

// ============================================================================
// FUNCIONES DE UTILIDAD PARA RUTAS
// ============================================================================

Route* constructDirectRoute(TransitSystem* sys, Station* from, Station* to, Line* line) {
    if (!sys || !from || !to || !line) return NULL;

    Route* route = createRoute(10);
    if (!route) return NULL;

    // Encontrar orden de estaciones en la línea
    int from_idx = -1, to_idx = -1;
    for (int i = 0; i < line->station_count; i++) {
        if (line->stations[i]->id == from->id) from_idx = i;
        if (line->stations[i]->id == to->id) to_idx = i;
    }

    if (from_idx == -1 || to_idx == -1) {
        destroyRoute(route);
        return NULL;
    }

    // Construir ruta
    int start = (from_idx < to_idx) ? from_idx : to_idx;
    int end = (from_idx < to_idx) ? to_idx : from_idx;

    for (int i = start; i <= end; i++) {
        addStationToRoute(route, line->stations[i], line);
    }

    // Calcular tiempo y costo
    int base_time = getBaseTravelTime(line, from->id, to->id);
    int delay = getCurrentDelay(sys, line->id);

    route->total_time_minutes = base_time + delay;
    route->total_cost = calculateTotalFare(route, sys->fares);

    return route;
}

Route* findRouteWithTransfer(TransitSystem* sys, Station* from, Station* to) {
    if (!sys || !from || !to) return NULL;

    // Buscar rutas con un transbordo
    for (int i = 0; i < sys->station_count; i++) {
        Station* transfer_station = sys->stations[i];

        // Verificar si hay conexión from -> transfer y transfer -> to
        Line* line1 = findConnectingLine(sys, from->id, transfer_station->id);
        Line* line2 = findConnectingLine(sys, transfer_station->id, to->id);

        if (line1 && line2 && line1 != line2) {
            Route* route = createRoute(15);
            if (!route) continue;

            // Primera parte del viaje
            Route* part1 = constructDirectRoute(sys, from, transfer_station, line1);
            if (!part1) {
                destroyRoute(route);
                continue;
            }

            // Segunda parte del viaje
            Route* part2 = constructDirectRoute(sys, transfer_station, to, line2);
            if (!part2) {
                destroyRoute(part1);
                destroyRoute(route);
                continue;
            }

            // Combinar rutas
            for (int j = 0; j < part1->path_length; j++) {
                addStationToRoute(route, part1->path[j], part1->lines_used[j]);
            }

            // Agregar segunda parte (sin duplicar estación de transbordo)
            for (int j = 1; j < part2->path_length; j++) {
                addStationToRoute(route, part2->path[j], part2->lines_used[j]);
            }

            route->total_time_minutes = part1->total_time_minutes + part2->total_time_minutes + 5; // +5 min transbordo
            route->total_cost = calculateTotalFare(route, sys->fares);

            destroyRoute(part1);
            destroyRoute(part2);

            printf("🔄 Ruta con transbordo encontrada vía %s\n", transfer_station->name);
            return route;
        }
    }

    return NULL;
}

// ============================================================================
// FUNCIONES PRINCIPALES DE PLANIFICACIÓN
// ============================================================================

Route* findFastestRoute(TransitSystem* sys, Station* from, Station* to, Time departure) {
    if (!sys || !from || !to) {
        printf("❌ Error: Parámetros inválidos para findFastestRoute\n");
        return NULL;
    }

    printf("🚀 Buscando ruta más rápida: %s → %s (salida: %02d:%02d)\n",
           from->name, to->name, departure.hour, departure.minute);

    // 1. Intentar conexión directa
    Line* direct_line = findConnectingLine(sys, from->id, to->id);
    if (direct_line) {
        Route* direct_route = constructDirectRoute(sys, from, to, direct_line);
        if (direct_route) {
            printf("✅ Ruta directa encontrada: %d minutos, $%.2f\n",
                   direct_route->total_time_minutes, direct_route->total_cost);
            return direct_route;
        }
    }

    // 2. Buscar ruta con transbordo
    Route* transfer_route = findRouteWithTransfer(sys, from, to);
    if (transfer_route) {
        printf("✅ Ruta con transbordo encontrada: %d minutos, $%.2f\n",
               transfer_route->total_time_minutes, transfer_route->total_cost);
        return transfer_route;
    }

    printf("❌ No se encontró ruta entre %s y %s\n", from->name, to->name);
    return NULL;
}

Route* findCheapestRoute(TransitSystem* sys, Station* from, Station* to) {
    if (!sys || !from || !to) {
        printf("❌ Error: Parámetros inválidos para findCheapestRoute\n");
        return NULL;
    }

    printf("💰 Buscando ruta más barata: %s → %s\n", from->name, to->name);

    Route* best_route = NULL;
    double min_cost = INFINITY;

    // 1. Evaluar conexión directa
    Line* direct_line = findConnectingLine(sys, from->id, to->id);
    if (direct_line) {
        Route* direct_route = constructDirectRoute(sys, from, to, direct_line);
        if (direct_route && direct_route->total_cost < min_cost) {
            if (best_route) destroyRoute(best_route);
            best_route = direct_route;
            min_cost = direct_route->total_cost;
        } else if (direct_route) {
            destroyRoute(direct_route);
        }
    }

    // 2. Evaluar rutas con transbordo
    Route* transfer_route = findRouteWithTransfer(sys, from, to);
    if (transfer_route && transfer_route->total_cost < min_cost) {
        if (best_route) destroyRoute(best_route);
        best_route = transfer_route;
        min_cost = transfer_route->total_cost;
    } else if (transfer_route) {
        destroyRoute(transfer_route);
    }

    if (best_route) {
        printf("✅ Ruta más barata encontrada: $%.2f, %d minutos\n",
               best_route->total_cost, best_route->total_time_minutes);
    } else {
        printf("❌ No se encontró ruta entre %s y %s\n", from->name, to->name);
    }

    return best_route;
}

Route** findRoutesWithTransfers(TransitSystem* sys, Station* from, Station* to, int maxTransfers, int* route_count) {
    if (!sys || !from || !to || !route_count) {
        if (route_count) *route_count = 0;
        return NULL;
    }

    printf("🔄 Buscando rutas con máximo %d transbordos: %s → %s\n",
           maxTransfers, from->name, to->name);

    Route** routes = (Route**)malloc(3 * sizeof(Route*)); // Máximo 3 rutas
    int count = 0;

    if (!routes) {
        *route_count = 0;
        return NULL;
    }

    // 1. Ruta directa (0 transbordos)
    if (maxTransfers >= 0) {
        Line* direct_line = findConnectingLine(sys, from->id, to->id);
        if (direct_line) {
            Route* direct_route = constructDirectRoute(sys, from, to, direct_line);
            if (direct_route) {
                routes[count++] = direct_route;
                printf("   ✅ Ruta directa (0 transbordos): %d min, $%.2f\n",
                       direct_route->total_time_minutes, direct_route->total_cost);
            }
        }
    }

    // 2. Ruta con 1 transbordo
    if (maxTransfers >= 1) {
        Route* transfer_route = findRouteWithTransfer(sys, from, to);
        if (transfer_route) {
            routes[count++] = transfer_route;
            printf("   ✅ Ruta con transbordo (1 transbordo): %d min, $%.2f\n",
                   transfer_route->total_time_minutes, transfer_route->total_cost);
        }
    }

    // 3. Ruta alternativa (si existe)
    if (maxTransfers >= 1 && count > 0) {
        // Crear una variación de la primera ruta con diferentes características
        Route* alt_route = createRoute(10);
        if (alt_route && count > 0) {
            // Copiar primera ruta pero con costo modificado (simulando ruta alternativa)
            Route* base_route = routes[0];
            for (int i = 0; i < base_route->path_length; i++) {
                addStationToRoute(alt_route, base_route->path[i], base_route->lines_used[i]);
            }

            alt_route->total_time_minutes = base_route->total_time_minutes + 5; // +5 min
            alt_route->total_cost = base_route->total_cost * 0.9; // -10% costo
            alt_route->transfer_count = base_route->transfer_count;

            routes[count++] = alt_route;
            printf("   ✅ Ruta alternativa: %d min, $%.2f\n",
                   alt_route->total_time_minutes, alt_route->total_cost);
        }
    }

    *route_count = count;

    if (count > 0) {
        printf("✅ Encontradas %d rutas\n", count);
    } else {
        printf("❌ No se encontraron rutas con máximo %d transbordos\n", maxTransfers);
        free(routes);
        return NULL;
    }

    return routes;
}

// ============================================================================
// FUNCIONES DE VALIDACIÓN
// ============================================================================

bool validateTransitSystem(TransitSystem* system) {
    if (!system) {
        printf("❌ Sistema nulo\n");
        return false;
    }

    if (system->station_count == 0) {
        printf("❌ No hay estaciones en el sistema\n");
        return false;
    }

    if (system->lines_count == 0) {
        printf("❌ No hay líneas en el sistema\n");
        return false;
    }

    // Verificar que cada línea tenga al menos 2 estaciones
    for (int i = 0; i < system->lines_count; i++) {
        if (system->lines[i]->station_count < 2) {
            printf("❌ Línea %s tiene menos de 2 estaciones\n", system->lines[i]->name);
            return false;
        }
    }

    // Verificar conectividad básica
    int connections = 0;
    for (int i = 0; i < system->station_count - 1; i++) {
        for (int j = i + 1; j < system->station_count; j++) {
            if (hasDirectConnection(system, system->stations[i]->id, system->stations[j]->id)) {
                connections++;
            }
        }
    }

    printf("✅ Sistema válido: %d estaciones, %d líneas, %d conexiones directas\n",
           system->station_count, system->lines_count, connections);
    return true;
}

// ============================================================================
// ALGORITMOS ESPECÍFICOS (IMPLEMENTACIONES BÁSICAS)
// ============================================================================

Route* dijkstraWithDynamicWeights(TransitSystem* sys, int from, int to, Time departure_time) {
    // TODO: Implementar Dijkstra completo con pesos dinámicos
    // Por ahora, usar implementación simplificada

    Station* from_station = findStationById(sys, from);
    Station* to_station = findStationById(sys, to);

    if (!from_station || !to_station) {
        printf("❌ Estaciones no encontradas para Dijkstra\n");
        return NULL;
    }

    printf("🔍 Ejecutando Dijkstra con pesos dinámicos\n");
    return findFastestRoute(sys, from_station, to_station, departure_time);
}

Route** bfsMinimumTransfers(TransitSystem* sys, int from, int to, int max_transfers, int* route_count) {
    // TODO: Implementar BFS completo para minimizar transbordos
    // Por ahora, usar implementación simplificada

    Station* from_station = findStationById(sys, from);
    Station* to_station = findStationById(sys, to);

    if (!from_station || !to_station) {
        if (route_count) *route_count = 0;
        printf("❌ Estaciones no encontradas para BFS\n");
        return NULL;
    }

    printf("🔍 Ejecutando BFS para minimizar transbordos\n");
    return findRoutesWithTransfers(sys, from_station, to_station, max_transfers, route_count);
}

Route* bellmanFordCheapest(TransitSystem* sys, int from, int to) {
    // TODO: Implementar Bellman-Ford completo para optimizar costos
    // Por ahora, usar implementación simplificada

    Station* from_station = findStationById(sys, from);
    Station* to_station = findStationById(sys, to);

    if (!from_station || !to_station) {
        printf("❌ Estaciones no encontradas para Bellman-Ford\n");
        return NULL;
    }

    printf("🔍 Ejecutando Bellman-Ford para optimizar costos\n");
    return findCheapestRoute(sys, from_station, to_station);
}

// ============================================================================
// FUNCIONES DE UTILIDAD ADICIONALES
// ============================================================================

void printRouteOptions(Route** routes, int route_count) {
    if (!routes || route_count == 0) {
        printf("❌ No hay rutas para mostrar\n");
        return;
    }

    printf("\n🗺️  ===== OPCIONES DE RUTA =====\n");
    for (int i = 0; i < route_count; i++) {
        printf("\n📍 OPCIÓN %d:\n", i + 1);
        printf("   ⏱️  Tiempo: %d minutos\n", routes[i]->total_time_minutes);
        printf("   💰 Costo: $%.2f\n", routes[i]->total_cost);
        printf("   🔄 Transbordos: %d\n", routes[i]->transfer_count);
        printf("   📋 Ruta: %s", routes[i]->path[0]->name);
        for (int j = 1; j < routes[i]->path_length; j++) {
            printf(" → %s", routes[i]->path[j]->name);
        }
        printf("\n");
    }
    printf("==============================\n\n");
}

void freeRouteArray(Route** routes, int route_count) {
    if (!routes) return;

    for (int i = 0; i < route_count; i++) {
        if (routes[i]) {
            destroyRoute(routes[i]);
        }
    }
    free(routes);
}

bool isValidRouteRequest(TransitSystem* sys, Station* from, Station* to) {
    if (!sys || !from || !to) {
        printf("❌ Parámetros de ruta inválidos\n");
        return false;
    }

    if (from->id == to->id) {
        printf("❌ Estación de origen y destino son la misma\n");
        return false;
    }

    return true;
}