//
// Created by administrador on 5/28/25.
//

#include "schedule.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Schedule* createSchedule(int line_id, int frequency_minutes, Time first, Time last) {
    Schedule* schedule = (Schedule*)malloc(sizeof(Schedule));
    if (!schedule) return NULL;

    schedule->line_id = line_id;
    schedule->frequency_minutes = frequency_minutes;
    schedule->first_service = first;
    schedule->last_service = last;
    schedule->capacity_departures = 100;
    schedule->departure_count = 0;

    schedule->departures = (Time*)malloc(schedule->capacity_departures * sizeof(Time));
    if (!schedule->departures) {
        free(schedule);
        return NULL;
    }

    printf("📅 Horario creado para línea %d: %02d:%02d - %02d:%02d (cada %d min)\n",
           line_id, first.hour, first.minute, last.hour, last.minute, frequency_minutes);

    return schedule;
}

void destroySchedule(Schedule* schedule) {
    if (!schedule) return;

    free(schedule->departures);
    free(schedule);
}

bool addDepartureTime(Schedule* schedule, Time departure) {
    if (!schedule || schedule->departure_count >= schedule->capacity_departures) {
        return false;
    }

    schedule->departures[schedule->departure_count++] = departure;
    printf("🕐 Salida agregada: %02d:%02d\n", departure.hour, departure.minute);
    return true;
}

Time getNextDeparture(Schedule* schedule, Time current_time) {
    Time invalid_time = {-1, -1};
    if (!schedule) return invalid_time;

    int current_minutes = current_time.hour * 60 + current_time.minute;

    for (int i = 0; i < schedule->departure_count; i++) {
        Time departure = schedule->departures[i];
        int departure_minutes = departure.hour * 60 + departure.minute;

        if (departure_minutes >= current_minutes) {
            return departure;
        }
    }

    // Si no hay más salidas, devolver primera del día siguiente
    if (schedule->departure_count > 0) {
        return schedule->departures[0];
    }

    return invalid_time;
}

bool isServiceActive(Schedule* schedule, Time current_time) {
    if (!schedule) return false;

    int current_minutes = current_time.hour * 60 + current_time.minute;
    int first_minutes = schedule->first_service.hour * 60 + schedule->first_service.minute;
    int last_minutes = schedule->last_service.hour * 60 + schedule->last_service.minute;

    return (current_minutes >= first_minutes && current_minutes <= last_minutes);
}

int getWaitTime(Schedule* schedule, Time current_time) {
    if (!schedule) return -1;

    Time next = getNextDeparture(schedule, current_time);
    if (next.hour == -1) return -1;

    int current_minutes = current_time.hour * 60 + current_time.minute;
    int next_minutes = next.hour * 60 + next.minute;

    int wait = next_minutes - current_minutes;
    if (wait < 0) wait += 24 * 60; // Próximo día

    return wait;
}

void generateSchedule(Schedule* schedule) {
    if (!schedule) return;

    printf("🕐 Generando horarios automáticos...\n");
    schedule->departure_count = 0;

    Time current = schedule->first_service;

    while (true) {
        addDepartureTime(schedule, current);

        current.minute += schedule->frequency_minutes;
        if (current.minute >= 60) {
            current.hour += current.minute / 60;
            current.minute %= 60;
        }

        int current_total = current.hour * 60 + current.minute;
        int last_total = schedule->last_service.hour * 60 + schedule->last_service.minute;

        if (current_total > last_total) break;
    }

    printf("✅ Generados %d horarios de salida\n", schedule->departure_count);
}

void printSchedule(Schedule* schedule) {
    if (!schedule) return;

    printf("\n📅 ===== HORARIO LÍNEA %d =====\n", schedule->line_id);
    printf("🕐 Servicio: %02d:%02d - %02d:%02d\n",
           schedule->first_service.hour, schedule->first_service.minute,
           schedule->last_service.hour, schedule->last_service.minute);
    printf("⏱️ Frecuencia: cada %d minutos\n", schedule->frequency_minutes);
    printf("📋 Salidas programadas (%d):\n", schedule->departure_count);

    for (int i = 0; i < schedule->departure_count; i++) {
        printf("   %02d:%02d", schedule->departures[i].hour, schedule->departures[i].minute);
        if ((i + 1) % 8 == 0) printf("\n"); // Nueva línea cada 8 horarios
        else if (i < schedule->departure_count - 1) printf(" | ");
    }
    printf("\n=============================\n\n");
}

