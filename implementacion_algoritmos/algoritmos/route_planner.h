
// ============================================================================
// algoritmos/route_planner.h - Planificador de rutas
// ============================================================================
#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include "../core/transit_system.h"
#include "../core/station.h"
#include "../core/route.h"
#include "../core/line.h"
#include "../utils/time_utils.h"

// Funciones principales de planificación de rutas
Route* findFastestRoute(TransitSystem* sys, Station* from, Station* to, Time departure);
Route* findCheapestRoute(TransitSystem* sys, Station* from, Station* to);
Route** findRoutesWithTransfers(TransitSystem* sys, Station* from, Station* to, int maxTransfers, int* route_count);

// Funciones auxiliares de búsqueda
Station* findStationById(TransitSystem* system, int id);
Station* findStationByName(TransitSystem* system, const char* name);
Line* findLineById(TransitSystem* system, int id);
Line* findLineByName(TransitSystem* system, const char* name);

// Funciones de validación y utilidad
bool validateTransitSystem(TransitSystem* system);
bool hasDirectConnection(TransitSystem* system, int from_station, int to_station);
Line* findConnectingLine(TransitSystem* system, int from_station, int to_station);
int getBaseTravelTime(Line* line, int from_station, int to_station);

// Funciones para algoritmos específicos (se implementarán después)
Route* dijkstraWithDynamicWeights(TransitSystem* sys, int from, int to, Time departure_time);
Route** bfsMinimumTransfers(TransitSystem* sys, int from, int to, int max_transfers, int* route_count);
Route* bellmanFordCheapest(TransitSystem* sys, int from, int to);

#endif //ROUTE_PLANNER_H
