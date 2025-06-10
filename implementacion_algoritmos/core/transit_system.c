// ============================================================================
// core/transit_system.c - Implementación del sistema principal
// ============================================================================
#include "transit_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// CREACIÓN Y DESTRUCCIÓN DEL SISTEMA
// ============================================================================

TransitSystem* createTransitSystem(int max_stations, int max_lines) {
    TransitSystem* system = (TransitSystem*)malloc(sizeof(TransitSystem));
    if (!system) {
        printf("❌ Error: No se pudo crear el sistema de transporte\n");
        return NULL;
    }

    // Inicializar el grafo usando tu implementación existente (NO dirigido)
    system->transit_network = crearGraph(max_stations, false);
    if (!system->transit_network) {
        printf("❌ Error: No se pudo crear el grafo del sistema\n");
        free(system);
        return NULL;
    }

    // Inicializar arrays dinámicos
    system->stations = (Station**)malloc(max_stations * sizeof(Station*));
    system->lines = (Line**)malloc(max_lines * sizeof(Line*));
    system->schedules = (Schedule**)malloc(max_lines * sizeof(Schedule*));
    system->delays = (RealTimeData**)malloc(max_lines * sizeof(RealTimeData*));

    if (!system->stations || !system->lines || !system->schedules || !system->delays) {
        printf("❌ Error: No se pudo asignar memoria para los arrays del sistema\n");
        destroyTransitSystem(system);
        return NULL;
    }

    // Inicializar contadores (USANDO NOMBRES CONSISTENTES)
    system->station_count = 0;
    system->lines_count = 0;        // Usar el nombre de tu header
    system->schedules_count = 0;    // Usar el nombre de tu header
    system->delay_count = 0;        // Este está correcto en tu header
    system->delays_count = 0;       // También está en tu header

    // Inicializar capacidades
    system->capacity_stations = max_stations;
    system->capacity_lines = max_lines;
    system->capacity_schedules = max_lines;
    system->capacity_delays = max_lines;

    // Inicializar matriz de precios (por defecto 5 zonas)
    system->fares = createPriceMatrix(5, 2.50, 0.50);
    if (!system->fares) {
        printf("⚠️ Advertencia: No se pudo crear matriz de precios\n");
    }

    printf("✅ Sistema de transporte creado con éxito\n");
    printf("   - Máximo estaciones: %d\n", max_stations);
    printf("   - Máximo líneas: %d\n", max_lines);
    printf("   - Grafo: NO dirigido (bidireccional)\n");
    printf("   - Matriz de precios: 5 zonas, tarifa base $2.50\n");

    return system;
}

void destroyTransitSystem(TransitSystem* system) {
    if (!system) return;

    printf("🗑️ Destruyendo sistema de transporte...\n");

    // Destruir todas las estaciones
    if (system->stations) {
        for (int i = 0; i < system->station_count; i++) {
            if (system->stations[i]) {
                destroyStation(system->stations[i]);
            }
        }
        free(system->stations);
    }

    // Destruir todas las líneas
    if (system->lines) {
        for (int i = 0; i < system->lines_count; i++) {
            if (system->lines[i]) {
                destroyLine(system->lines[i]);
            }
        }
        free(system->lines);
    }

    // Destruir horarios
    if (system->schedules) {
        for (int i = 0; i < system->schedules_count; i++) {
            if (system->schedules[i]) {
                destroySchedule(system->schedules[i]);
            }
        }
        free(system->schedules);
    }

    // Destruir datos de tiempo real
    if (system->delays) {
        for (int i = 0; i < system->delay_count; i++) {
            if (system->delays[i]) {
                destroyRealTimeData(system->delays[i]);
            }
        }
        free(system->delays);
    }

    // Destruir matriz de precios
    if (system->fares) {
        destroyPriceMatrix(system->fares);
    }

    // Destruir grafo
    if (system->transit_network) {
        destruirGraph(system->transit_network);
    }

    free(system);
    printf("✅ Sistema de transporte destruido completamente\n");
}

// ============================================================================
// GESTIÓN DE ESTACIONES
// ============================================================================

bool addStationToSystem(TransitSystem* system, Station* station) {
    if (!system || !station) {
        printf("❌ Error: Sistema o estación nulos\n");
        return false;
    }

    if (system->station_count >= system->capacity_stations) {
        printf("❌ Error: Capacidad máxima de estaciones alcanzada (%d)\n",
               system->capacity_stations);
        return false;
    }

    // Verificar que no existe una estación con el mismo ID
    for (int i = 0; i < system->station_count; i++) {
        if (system->stations[i]->id == station->id) {
            printf("❌ Error: Ya existe una estación con ID %d (%s)\n",
                   station->id, system->stations[i]->name);
            return false;
        }
    }

    // Verificar que no existe una estación con el mismo nombre
    for (int i = 0; i < system->station_count; i++) {
        if (strcmp(system->stations[i]->name, station->name) == 0) {
            printf("❌ Error: Ya existe una estación con nombre '%s'\n", station->name);
            return false;
        }
    }

    // Agregar estación al sistema
    system->stations[system->station_count] = station;
    system->station_count++;

    // Agregar vértice al grafo
    if (!addVertex(system->transit_network, station->id)) {
        printf("⚠️ Advertencia: No se pudo agregar vértice al grafo\n");
    }

    printf("➕ Estación agregada: %s (ID: %d, Zona: %d, %s)\n",
           station->name, station->id, station->zone,
           station->accesible ? "Accesible" : "No accesible");

    return true;
}

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

// ============================================================================
// GESTIÓN DE LÍNEAS
// ============================================================================

bool addLineToSystem(TransitSystem* system, Line* line) {
    if (!system || !line) {
        printf("❌ Error: Sistema o línea nulos\n");
        return false;
    }

    if (system->lines_count >= system->capacity_lines) {
        printf("❌ Error: Capacidad máxima de líneas alcanzada (%d)\n",
               system->capacity_lines);
        return false;
    }

    // Verificar que no existe una línea con el mismo ID
    for (int i = 0; i < system->lines_count; i++) {
        if (system->lines[i]->id == line->id) {
            printf("❌ Error: Ya existe una línea con ID %d (%s)\n",
                   line->id, system->lines[i]->name);
            return false;
        }
    }

    // Verificar que no existe una línea con el mismo nombre
    for (int i = 0; i < system->lines_count; i++) {
        if (strcmp(system->lines[i]->name, line->name) == 0) {
            printf("❌ Error: Ya existe una línea con nombre '%s'\n", line->name);
            return false;
        }
    }

    // Verificar que la línea tenga al menos 2 estaciones
    if (line->station_count < 2) {
        printf("❌ Error: La línea debe tener al menos 2 estaciones\n");
        return false;
    }

    // Agregar línea al sistema
    system->lines[system->lines_count] = line;
    system->lines_count++;

    // Agregar conexiones al grafo (bidireccionales)
    int connections_added = 0;
    for (int i = 0; i < line->station_count - 1; i++) {
        if (line->stations[i] && line->stations[i + 1]) {
            int from = line->stations[i]->id;
            int to = line->stations[i + 1]->id;
            int weight = line->travel_times[i];

            // Agregar arista bidireccional
            if (addEdgeList(system->transit_network, from, to, weight) &&
                addEdgeList(system->transit_network, to, from, weight)) {
                connections_added++;
            }
        }
    }

    const char* type_names[] = {"Metro", "Autobús", "Tren"};
    printf("➕ Línea agregada: %s (%s, Color: %s)\n",
           line->name, type_names[line->type], line->color);
    printf("   📍 %d estaciones, %d conexiones bidireccionales\n",
           line->station_count, connections_added);

    return true;
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

// ============================================================================
// GESTIÓN DE HORARIOS
// ============================================================================

bool addScheduleToSystem(TransitSystem* system, Schedule* schedule) {
    if (!system || !schedule) {
        printf("❌ Error: Sistema o horario nulos\n");
        return false;
    }

    if (system->schedules_count >= system->capacity_schedules) {
        printf("❌ Error: Capacidad máxima de horarios alcanzada (%d)\n",
               system->capacity_schedules);
        return false;
    }

    // Verificar que existe la línea para este horario
    Line* line = findLineById(system, schedule->line_id);
    if (!line) {
        printf("❌ Error: No existe línea con ID %d para el horario\n", schedule->line_id);
        return false;
    }

    // Verificar que no existe horario para esta línea
    for (int i = 0; i < system->schedules_count; i++) {
        if (system->schedules[i]->line_id == schedule->line_id) {
            printf("❌ Error: Ya existe horario para línea ID %d (%s)\n",
                   schedule->line_id, line->name);
            return false;
        }
    }

    system->schedules[system->schedules_count] = schedule;
    system->schedules_count++;

    printf("➕ Horario agregado para línea %s (ID: %d)\n", line->name, schedule->line_id);
    printf("   🕐 Servicio: %02d:%02d - %02d:%02d, frecuencia: %d min\n",
           schedule->first_service.hour, schedule->first_service.minute,
           schedule->last_service.hour, schedule->last_service.minute,
           schedule->frequency_minutes);

    return true;
}

// ============================================================================
// VISUALIZACIÓN DEL SISTEMA
// ============================================================================

void printTransitSystem(TransitSystem* system) {
    if (!system) {
        printf("❌ Sistema nulo\n");
        return;
    }

    printf("\n🚇 ===== SISTEMA DE TRANSPORTE =====\n");
    printf("📊 ESTADÍSTICAS GENERALES:\n");
    printf("   📍 Estaciones: %d/%d\n", system->station_count, system->capacity_stations);
    printf("   🚌 Líneas: %d/%d\n", system->lines_count, system->capacity_lines);
    printf("   📅 Horarios: %d/%d\n", system->schedules_count, system->capacity_schedules);
    printf("   ⚠️ Retrasos activos: %d/%d\n", system->delay_count, system->capacity_delays);

    if (system->station_count > 0) {
        printf("\n📍 ESTACIONES:\n");
        for (int i = 0; i < system->station_count; i++) {
            Station* s = system->stations[i];
            printf("   %d. %s (ID: %d, Zona: %d) - %.6f, %.6f %s\n",
                   i + 1, s->name, s->id, s->zone, s->latitude, s->longitude,
                   s->accesible ? "♿" : "");
        }
    }

    if (system->lines_count > 0) {
        printf("\n🚌 LÍNEAS:\n");
        for (int i = 0; i < system->lines_count; i++) {
            Line* l = system->lines[i];
            const char* types[] = {"Metro", "Autobús", "Tren"};
            printf("   %d. %s (ID: %d, %s, %s) - %d estaciones\n",
                   i + 1, l->name, l->id, types[l->type], l->color, l->station_count);

            // Mostrar ruta de la línea
            if (l->station_count > 0) {
                printf("      Ruta: %s", l->stations[0]->name);
                for (int j = 1; j < l->station_count; j++) {
                    printf(" → %s", l->stations[j]->name);
                }
                printf("\n");
            }
        }
    }

    if (system->schedules_count > 0) {
        printf("\n📅 HORARIOS:\n");
        for (int i = 0; i < system->schedules_count; i++) {
            Schedule* sch = system->schedules[i];
            Line* line = findLineById(system, sch->line_id);
            printf("   %s: %02d:%02d - %02d:%02d (cada %d min)\n",
                   line ? line->name : "Línea desconocida",
                   sch->first_service.hour, sch->first_service.minute,
                   sch->last_service.hour, sch->last_service.minute,
                   sch->frequency_minutes);
        }
    }

    if (system->fares) {
        printf("\n💰 TARIFAS:\n");
        printf("   Base: $%.2f | Transbordo: $%.2f | Zonas: %d\n",
               system->fares->base_fare, system->fares->transfer_penalty,
               system->fares->zone_count);
    }

    printf("=====================================\n\n");
}

// ============================================================================
// VALIDACIÓN DEL SISTEMA
// ============================================================================

bool validateTransitSystem(TransitSystem* system) {
    if (!system) {
        printf("❌ Sistema nulo\n");
        return false;
    }

    bool is_valid = true;

    // Verificar estaciones
    if (system->station_count == 0) {
        printf("❌ No hay estaciones en el sistema\n");
        is_valid = false;
    }

    // Verificar líneas
    if (system->lines_count == 0) {
        printf("❌ No hay líneas en el sistema\n");
        is_valid = false;
    }

    // Verificar que cada línea tenga al menos 2 estaciones
    for (int i = 0; i < system->lines_count; i++) {
        Line* line = system->lines[i];
        if (line->station_count < 2) {
            printf("❌ Línea %s tiene menos de 2 estaciones (%d)\n",
                   line->name, line->station_count);
            is_valid = false;
        }
    }

    // Verificar conectividad básica
    if (system->station_count > 1) {
        // Contar conexiones directas
        int direct_connections = 0;
        for (int i = 0; i < system->station_count - 1; i++) {
            for (int j = i + 1; j < system->station_count; j++) {
                // Verificar si hay conexión directa usando el grafo
                if (hasEdge(system->transit_network,
                           system->stations[i]->id,
                           system->stations[j]->id)) {
                    direct_connections++;
                }
            }
        }

        if (direct_connections == 0) {
            printf("⚠️ Advertencia: No hay conexiones entre estaciones\n");
        } else {
            printf("ℹ️ Conexiones directas: %d\n", direct_connections);
        }
    }

    // Verificar matriz de precios
    if (!system->fares) {
        printf("⚠️ Advertencia: No hay matriz de precios configurada\n");
    }

    if (is_valid) {
        printf("✅ Sistema válido: %d estaciones, %d líneas, %d horarios\n",
               system->station_count, system->lines_count, system->schedules_count);
    }

    return is_valid;
}

// ============================================================================
// FUNCIONES DE UTILIDAD
// ============================================================================

int getSystemStationCount(TransitSystem* system) {
    return system ? system->station_count : 0;
}

int getSystemLineCount(TransitSystem* system) {
    return system ? system->lines_count : 0;
}

int getSystemScheduleCount(TransitSystem* system) {
    return system ? system->schedules_count : 0;
}

bool isSystemEmpty(TransitSystem* system) {
    return !system || (system->station_count == 0 && system->lines_count == 0);
}

void printSystemSummary(TransitSystem* system) {
    if (!system) {
        printf("❌ Sistema nulo\n");
        return;
    }

    printf("📊 RESUMEN DEL SISTEMA:\n");
    printf("   🚉 %d estaciones | 🚇 %d líneas | 📅 %d horarios\n",
           system->station_count, system->lines_count, system->schedules_count);

    if (system->station_count > 0 && system->lines_count > 0) {
        printf("   ✅ Sistema operativo\n");
    } else {
        printf("   ⚠️ Sistema incompleto\n");
    }
}