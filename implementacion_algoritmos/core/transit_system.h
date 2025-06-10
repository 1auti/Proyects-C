#ifndef TRANSIT_SYSTEM_H
#define TRANSIT_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../graph/graph.h"

// Definición de constantes
#define MAX_STATION_NAME 50
#define MAX_LINE_NAME 30
#define MAX_STATIONS 1000

// Estructura para representar una estación
typedef struct Station {
 int id;
 char name[MAX_STATION_NAME];
 double latitude;
 double longitude;
 bool is_active;
} Station;

// Estructura para representar una línea de transporte
typedef struct TransitLine {
 int line_id;
 char line_name[MAX_LINE_NAME];
 int* stations;
 int num_stations;
 int capacity;
 bool is_operational;
} TransitLine;

// Estructura principal del sistema de tránsito
typedef struct TransitSystem {
 Graph* transit_network;
 Station* stations;
 TransitLine* lines;
 int num_stations;
 int num_lines;
 int max_stations;
 int max_lines;
} TransitSystem;

// Funciones para el sistema de tránsito
TransitSystem* createTransitSystem(int max_stations, int max_lines);
void destroyTransitSystem(TransitSystem* system);

// Funciones para manejo de estaciones
bool addStation(TransitSystem* system, int station_id, const char* name,
                double lat, double lng);
bool removeStation(TransitSystem* system, int station_id);
Station* findStation(TransitSystem* system, int station_id);
void printStation(const Station* station);

// Funciones para manejo de líneas
bool addLine(TransitSystem* system, int line_id, const char* line_name);
bool addStationToLine(TransitSystem* system, int line_id, int station_id);
bool addLineToSystem(TransitSystem* system, int from, int to, int weight);
TransitLine* findLine(TransitSystem* system, int line_id);

// Funciones de utilidad
void printTransitSystem(TransitSystem* system);
bool isValidStation(TransitSystem* system, int station_id);
int getStationCount(TransitSystem* system);
int getLineCount(TransitSystem* system);

#endif // TRANSIT_SYSTEM_H