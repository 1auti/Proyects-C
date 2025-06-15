//
// Created by administrador on 5/28/25.
//

#ifndef STATION_H
#define STATION_H
#include <stdbool.h>
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Station{

int id;
char name[100];
double latitude;
double longitude;
int zone; // zona tarifaria;
bool accesible; // si es accesible para discapacitados
char ** lines; // lineas que pasan por esa estacion;
int line_count; // numero de lineas
int capacity_lines; // capacidad del array de lineas
}Station;

Station * createStation(int id, const char * name, double lat, double lon, int zone, bool accesible);

void destroyStation(Station * station);

bool addLineToStation(Station * station, const char * line_name);

void printStation(Station* station);

#endif //STATION_H
