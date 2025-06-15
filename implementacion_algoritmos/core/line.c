//
// Created by administrador on 5/28/25.
//

#include "line.h"

Line* createLine(int id, const char* name, const char* color, int type) {
    Line * line = (Line*)malloc(sizeof(Line));
    if(!line) return NULL;

    line->id = id;
    strncpy(line->name,name,sizeof(line->name));
    line->name[sizeof(line->name) - 1] = '\0';
    strncpy(line->color, color, sizeof(line->color) - 1);
    line->color[sizeof(line->color) - 1] = '\0';
    line->type = type;

    // Inicializar arrays dinámicos
    line->capacity_stations = 50;
    line->stations = (Station**)malloc(line->capacity_stations * sizeof(Station*));
    line->travel_times = (int*)malloc(line->capacity_stations * sizeof(int));
    line->station_count = 0;

    if (!line->stations || !line->travel_times) {
        free(line->stations);
        free(line->travel_times);
        free(line);
        return NULL;
    }

    return line;

}
void destroyLine(Line* line) {
    if (!line) return;

    free(line->stations);
    free(line->travel_times);
    free(line);
}

bool addStationToLine(Line* line, Station* station, int travel_time_to_next) {
    if (!line || !station || line->station_count >= line->capacity_stations) {
        return false;
    }

    // Verificar que la estación no esté ya en la línea
    for (int i = 0; i < line->station_count; i++) {
        if (line->stations[i]->id == station->id) {
            return false; // Ya existe
        }
    }

    line->stations[line->station_count] = station;
    line->travel_times[line->station_count] = travel_time_to_next;
    line->station_count++;

    // Agregar línea a la estación
    addLineToStation(station, line->name);

    return true;
}

void printLine(Line* line) {
    if (!line) return;

    const char* type_names[] = {"Metro", "Autobús", "Tren"};
    printf("🚇 Línea: %s (ID: %d)\n", line->name, line->id);
    printf("   🎨 Color: %s\n", line->color);
    printf("   🚌 Tipo: %s\n", type_names[line->type]);
    printf("   📍 Estaciones (%d):\n", line->station_count);

    for (int i = 0; i < line->station_count; i++) {
        printf("      %d. %s", i + 1, line->stations[i]->name);
        if (i < line->station_count - 1) {
            printf(" → %d min", line->travel_times[i]);
        }
        printf("\n");
    }
}

// ============================================================================
// route.c - Implementación de funciones de ruta
// ============================================================================
#include "route.h"
#include <stdlib.h>
#include <stdio.h>

Route* createRoute(int initial_capacity) {
    Route* route = (Route*)malloc(sizeof(Route));
    if (!route) return NULL;

    route->capacity_path = initial_capacity;
    route->capacity_lines = initial_capacity;

    route->path = (Station**)malloc(route->capacity_path * sizeof(Station*));
    route->lines_used = (Line**)malloc(route->capacity_lines * sizeof(Line*));

    if (!route->path || !route->lines_used) {
        free(route->path);
        free(route->lines_used);
        free(route);
        return NULL;
    }

    route->path_length = 0;
    route->transfer_count = 0;
    route->total_cost = 0.0;
    route->total_time_minutes = 0;
    route->departure_time = NULL;
    route->arrival_time = NULL;

    return route;
}

void destroyRoute(Route* route) {
    if (!route) return;

    free(route->path);
    free(route->lines_used);
    if (route->departure_time) free(route->departure_time);
    if (route->arrival_time) free(route->arrival_time);
    free(route);
}

bool addStationToRoute(Route* route, Station* station, Line* line) {
    if (!route || !station || route->path_length >= route->capacity_path) {
        return false;
    }

    route->path[route->path_length] = station;

    // Solo agregar línea si es diferente a la anterior
    if (route->path_length == 0 ||
        (route->path_length > 0 && route->lines_used[route->path_length - 1] != line)) {

        if (route->path_length > 0) {
            route->transfer_count++;
        }

        route->lines_used[route->path_length] = line;
    } else {
        route->lines_used[route->path_length] = route->lines_used[route->path_length - 1];
    }

    route->path_length++;
    return true;
}

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

    printf("\n📋 Recorrido:\n");
    Line* current_line = NULL;

    for (int i = 0; i < route->path_length; i++) {
        Station* station = route->path[i];
        Line* line = route->lines_used[i];

        if (current_line != line) {
            if (current_line != NULL) {
                printf("   🔄 TRANSBORDO\n");
            }
            printf("   🚇 Línea: %s\n", line->name);
            current_line = line;
        }

        printf("   %d. 🚉 %s", i + 1, station->name);
        if (i < route->path_length - 1) {
            // Buscar tiempo de viaje entre estaciones consecutivas
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
