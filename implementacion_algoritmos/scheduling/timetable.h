//
// Created by administrador on 5/28/25.
//

#ifndef TIMETABLE_H
#define TIMETABLE_H

#include "../utils/time_utils.h"
#include "../core/station.h"
#include <stdbool.h>
#include "schedule.h"

// Estructura de tabla de horarios detallada
typedef struct Timetable {
    int station_count;           // Número de estaciones
    Time** departure_times;      // Array 2D: [station][departure_index]
    int* departure_counts;       // Número de salidas por estación
    int* capacity_per_station;   // Capacidad de cada estación
    char** station_names;        // Nombres de estaciones para referencia
} Timetable;

// Entrada individual de horario
typedef struct TimetableEntry {
    int station_id;
    Time departure;
    Time arrival;
    int platform;
    char status[50];             // "On time", "Delayed", "Cancelled"
} TimetableEntry;

// Funciones principales
Timetable* createTimetable(int station_count);
void destroyTimetable(Timetable* table);

// Gestión de horarios por estación
bool addDepartureToStation(Timetable* table, int station_index, Time departure);
bool removeDepartureFromStation(Timetable* table, int station_index, Time departure);
Time getDepartureAt(Timetable* table, int station_index, Time after);
Time getArrivalTime(Timetable* table, int from_station, int to_station, Time departure);

// Consultas avanzadas
TimetableEntry* getNextDepartures(Timetable* table, int station_index, int count);
bool isStationServiceActive(Timetable* table, int station_index, Time current_time);
int getWaitTimeAtStation(Timetable* table, int station_index, Time current_time);

// Utilidades
void printTimetable(Timetable* table);
void printStationSchedule(Timetable* table, int station_index);
bool validateTimetable(Timetable* table);

// Integración con Schedule
bool importFromSchedule(Timetable* table, Schedule* schedule, int station_index);
Schedule* exportToSchedule(Timetable* table, int station_index, int line_id);


#endif //TIMETABLE_H
