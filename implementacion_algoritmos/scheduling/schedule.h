//
// Created by administrador on 5/28/25.
//

#ifndef SCHEDULE_H
#define SCHEDULE_H
#include "../utils/time_utils.h"
#include <stdbool.h>

// Horarios de la linea (tu estructura ya existente)
typedef struct Schedule {
    int line_id;
    Time* departures;            // Array de horarios de salida
    int departure_count;         // Numero de salidas
    int frequency_minutes;       // Frecuencuas de minutos
    Time first_service;          // Primer servicio del dia
    Time last_service;           // Ultimo servicio del dia
    int capacity_departures;     // Capacidad del array de salidas
} Schedule;

// Funciones de gestión de horarios
Schedule* createSchedule(int line_id, int frequency_minutes, Time first, Time last);
void destroySchedule(Schedule* schedule);

// Gestión de salidas
bool addDepartureTime(Schedule* schedule, Time departure);
bool removeDepartureTime(Schedule* schedule, Time departure);
void clearAllDepartures(Schedule* schedule);

// Consultas de horarios
Time getNextDeparture(Schedule* schedule, Time current_time);
Time getPreviousDeparture(Schedule* schedule, Time current_time);
bool isServiceActive(Schedule* schedule, Time current_time);
int getWaitTime(Schedule* schedule, Time current_time);

// Generación automática de horarios
void generateSchedule(Schedule* schedule);
void generateWeekdaySchedule(Schedule* schedule);
void generateWeekendSchedule(Schedule* schedule);

// Utilidades
void printSchedule(Schedule* schedule);
bool isValidSchedule(Schedule* schedule);
int getTotalServiceHours(Schedule* schedule);

#endif //SCHEDULE_H
