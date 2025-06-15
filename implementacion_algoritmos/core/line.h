//
// Created by administrador on 5/28/25.
//

#ifndef LINE_H
#define LINE_H
#include "station.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Metros, Bus, Tren
typedef struct Line{
int id;
char name[50];
char color[50];
int type; // 0: Metro 1: Autobus 2: Tren
Station ** stations; // Estaciones de las lineas
int station_count; // numerode estaciones
int * travel_times; // Tiempo entre estaciones consecutivas (minutos)
int capacity_stations; // Capacidad del array de estaciones

}Line;

Line* createLine(int id, const char* name, const char* color, int type);
void destroyLine(Line* line);
bool addStationToLine(Line* line, Station* station, int travel_time_to_next);
void printLine(Line* line);




#endif //LINE_H
