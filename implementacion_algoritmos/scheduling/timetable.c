//
// Created by administrador on 5/28/25.
//

#include "timetable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Timetable* createTimetable(int station_count) {
    Timetable* table = (Timetable*)malloc(sizeof(Timetable));
    if (!table) return NULL;

    table->station_count = station_count;
    table->departure_times = (Time**)malloc(station_count * sizeof(Time*));
    table->departure_counts = (int*)calloc(station_count, sizeof(int));
    table->capacity_per_station = (int*)malloc(station_count * sizeof(int));
    table->station_names = (char**)malloc(station_count * sizeof(char*));

    if (!table->departure_times || !table->departure_counts ||
        !table->capacity_per_station || !table->station_names) {
        free(table->departure_times);
        free(table->departure_counts);
        free(table->capacity_per_station);
        free(table->station_names);
        free(table);
        return NULL;
    }

    // Inicializar arrays por estación
    for (int i = 0; i < station_count; i++) {
        table->capacity_per_station[i] = 50; // 50 salidas por estación
        table->departure_times[i] = (Time*)malloc(50 * sizeof(Time));
        table->station_names[i] = (char*)malloc(100 * sizeof(char));

        if (!table->departure_times[i] || !table->station_names[i]) {
            // Limpiar memoria parcialmente asignada
            for (int j = 0; j <= i; j++) {
                free(table->departure_times[j]);
                free(table->station_names[j]);
            }
            free(table->departure_times);
            free(table->departure_counts);
            free(table->capacity_per_station);
            free(table->station_names);
            free(table);
            return NULL;
        }

        // Inicializar nombre de estación
        snprintf(table->station_names[i], 100, "Estación_%d", i + 1);
    }

    printf("📋 Tabla de horarios creada para %d estaciones\n", station_count);
    return table;
}

void destroyTimetable(Timetable* table) {
    if (!table) return;

    for (int i = 0; i < table->station_count; i++) {
        free(table->departure_times[i]);
        free(table->station_names[i]);
    }
    free(table->departure_times);
    free(table->departure_counts);
    free(table->capacity_per_station);
    free(table->station_names);
    free(table);
}

bool addDepartureToStation(Timetable* table, int station_index, Time departure) {
    if (!table || station_index < 0 || station_index >= table->station_count ||
        table->departure_counts[station_index] >= table->capacity_per_station[station_index]) {
        return false;
    }

    table->departure_times[station_index][table->departure_counts[station_index]] = departure;
    table->departure_counts[station_index]++;

    printf("🚉 Salida agregada a %s: %02d:%02d\n",
           table->station_names[station_index], departure.hour, departure.minute);
    return true;
}

bool removeDepartureFromStation(Timetable* table, int station_index, Time departure) {
    if (!table || station_index < 0 || station_index >= table->station_count) {
        return false;
    }

    for (int i = 0; i < table->departure_counts[station_index]; i++) {
        Time* dep = &table->departure_times[station_index][i];
        if (dep->hour == departure.hour && dep->minute == departure.minute) {
            // Mover elementos hacia atrás
            for (int j = i; j < table->departure_counts[station_index] - 1; j++) {
                table->departure_times[station_index][j] = table->departure_times[station_index][j + 1];
            }
            table->departure_counts[station_index]--;
            return true;
        }
    }
    return false;
}

Time getDepartureAt(Timetable* table, int station_index, Time after) {
    Time invalid_time = {-1, -1};

    if (!table || station_index < 0 || station_index >= table->station_count) {
        return invalid_time;
    }

    int after_minutes = after.hour * 60 + after.minute;

    for (int i = 0; i < table->departure_counts[station_index]; i++) {
        Time departure = table->departure_times[station_index][i];
        int dep_minutes = departure.hour * 60 + departure.minute;

        if (dep_minutes >= after_minutes) {
            return departure;
        }
    }

    return invalid_time;
}

Time getArrivalTime(Timetable* table, int from_station, int to_station, Time departure) {
    Time invalid_time = {-1, -1};

    if (!table || from_station < 0 || to_station < 0 ||
        from_station >= table->station_count || to_station >= table->station_count) {
        return invalid_time;
    }

    // Implementación simplificada: agregar tiempo fijo de viaje
    int travel_time = abs(to_station - from_station) * 3; // 3 minutos por estación

    Time arrival = departure;
    arrival.minute += travel_time;

    if (arrival.minute >= 60) {
        arrival.hour += arrival.minute / 60;
        arrival.minute %= 60;
    }

    return arrival;
}

TimetableEntry* getNextDepartures(Timetable* table, int station_index, int count) {
    if (!table || station_index < 0 || station_index >= table->station_count || count <= 0) {
        return NULL;
    }

    TimetableEntry* entries = (TimetableEntry*)malloc(count * sizeof(TimetableEntry));
    if (!entries) return NULL;

    int available = (count > table->departure_counts[station_index]) ?
                   table->departure_counts[station_index] : count;

    for (int i = 0; i < available; i++) {
        entries[i].station_id = station_index;
        entries[i].departure = table->departure_times[station_index][i];
        entries[i].arrival = entries[i].departure; // Simplificado
        entries[i].platform = (i % 3) + 1; // Plataformas 1-3
        strcpy(entries[i].status, "On time");
    }

    return entries;
}

bool isStationServiceActive(Timetable* table, int station_index, Time current_time) {
    if (!table || station_index < 0 || station_index >= table->station_count) {
        return false;
    }

    // Servicio activo si hay salidas programadas
    return table->departure_counts[station_index] > 0;
}

int getWaitTimeAtStation(Timetable* table, int station_index, Time current_time) {
    if (!table || station_index < 0 || station_index >= table->station_count) {
        return -1;
    }

    Time next_departure = getDepartureAt(table, station_index, current_time);
    if (next_departure.hour == -1) return -1;

    int current_minutes = current_time.hour * 60 + current_time.minute;
    int next_minutes = next_departure.hour * 60 + next_departure.minute;

    return next_minutes - current_minutes;
}

void printTimetable(Timetable* table) {
    if (!table) return;

    printf("\n📋 ===== TABLA DE HORARIOS =====\n");
    printf("🚉 Estaciones: %d\n", table->station_count);

    for (int i = 0; i < table->station_count; i++) {
        printf("\n📍 %s (%d salidas):\n",
               table->station_names[i], table->departure_counts[i]);

        for (int j = 0; j < table->departure_counts[i]; j++) {
            Time dep = table->departure_times[i][j];
            printf("   %02d:%02d", dep.hour, dep.minute);
            if ((j + 1) % 6 == 0) printf("\n");
            else if (j < table->departure_counts[i] - 1) printf(" | ");
        }
        if (table->departure_counts[i] % 6 != 0) printf("\n");
    }
    printf("=============================\n\n");
}

void printStationSchedule(Timetable* table, int station_index) {
    if (!table || station_index < 0 || station_index >= table->station_count) {
        printf("❌ Índice de estación inválido\n");
        return;
    }

    printf("\n🚉 ===== HORARIOS: %s =====\n", table->station_names[station_index]);
    printf("📋 Salidas programadas (%d):\n", table->departure_counts[station_index]);

    for (int i = 0; i < table->departure_counts[station_index]; i++) {
        Time dep = table->departure_times[station_index][i];
        printf("   %02d:%02d", dep.hour, dep.minute);
        if ((i + 1) % 8 == 0) printf("\n");
        else if (i < table->departure_counts[station_index] - 1) printf(" | ");
    }
    if (table->departure_counts[station_index] % 8 != 0) printf("\n");
    printf("===============================\n\n");
}

bool validateTimetable(Timetable* table) {
    if (!table) {
        printf("❌ Tabla de horarios nula\n");
        return false;
    }

    if (table->station_count <= 0) {
        printf("❌ No hay estaciones en la tabla\n");
        return false;
    }

    int total_departures = 0;
    for (int i = 0; i < table->station_count; i++) {
        total_departures += table->departure_counts[i];
    }

    if (total_departures == 0) {
        printf("❌ No hay salidas programadas\n");
        return false;
    }

    printf("✅ Tabla de horarios válida: %d estaciones, %d salidas totales\n",
           table->station_count, total_departures);
    return true;
}

bool importFromSchedule(Timetable* table, Schedule* schedule, int station_index) {
    if (!table || !schedule || station_index < 0 || station_index >= table->station_count) {
        return false;
    }

    // Limpiar salidas existentes
    table->departure_counts[station_index] = 0;

    // Importar salidas del schedule
    for (int i = 0; i < schedule->departure_count &&
         i < table->capacity_per_station[station_index]; i++) {
        table->departure_times[station_index][i] = schedule->departures[i];
        table->departure_counts[station_index]++;
    }

    printf("📥 Importados %d horarios desde schedule línea %d a %s\n",
           table->departure_counts[station_index], schedule->line_id,
           table->station_names[station_index]);

    return true;
}

Schedule* exportToSchedule(Timetable* table, int station_index, int line_id) {
    if (!table || station_index < 0 || station_index >= table->station_count ||
        table->departure_counts[station_index] == 0) {
        return NULL;
        }

    // Crear schedule básico
    Time first = table->departure_times[station_index][0];
    Time last = table->departure_times[station_index][table->departure_counts[station_index] - 1];

    Schedule* schedule = createSchedule(line_id, 10, first, last); // 10 min frecuencia por defecto
    if (!schedule) return NULL;

    // Exportar todas las salidas
    for (int i = 0; i < table->departure_counts[station_index]; i++) {
        addDepartureTime(schedule, table->departure_times[station_index][i]);
    }

    printf("📤 Exportados %d horarios de %s a schedule línea %d\n",
           schedule->departure_count, table->station_names[station_index], line_id);

    return schedule;
}