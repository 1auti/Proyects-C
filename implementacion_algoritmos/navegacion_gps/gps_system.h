//
// Created by administrador on 5/26/25.
//

#ifndef GPS_SYSTEM_H
#define GPS_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

// Incluir todas las estructuras y algoritmos
#include "../algoritmos/dijkstra.h"
#include "../algoritmos/bellman_ford.h"
#include "../algoritmos/dfs_bfs.h"
#include "../estructura_datos/hash_map.h"
#include "../estructura_datos/priority_queue.h"
#include "../utils/file_io.h"
#include "../utils/visualization.h"
#include "../algoritmos/cycle_detection.h"
#include "../graph/graph.h"


#define MAX_CITIES 1000
#define MAX_NAME_LENGTH 50
#define MAX_ROUTES 10
#define CACHE_SIZE 100
#define INF INT_MAX

// =================================================================
// Estructuras principales del GPS
// =================================================================

// Coordenadas geográficas
typedef struct {
    double latitude;
    double longitude;
} GeoCoordinate;

// Ciudad en el sistema
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    GeoCoordinate location;
    int population;
    char region[MAX_NAME_LENGTH];
    bool isActive;
} City;

// Carretera entre ciudades
typedef struct {
    int from;
    int to;
    double distance;        // Distancia en km
    double baseTime;        // Tiempo base en minutos
    double currentTime;     // Tiempo actual (con tráfico)
    double toll;           // Peaje (puede ser negativo para descuentos)
    char roadType[20];     // "highway", "urban", "rural", "scenic"
    int speedLimit;        // Límite de velocidad
    bool isClosed;         // Estado de la carretera
    time_t lastUpdate;     // Última actualización de tráfico
} Road;

// Ruta calculada
typedef struct {
    int* cityPath;         // Camino de ciudades
    int pathLength;        // Número de ciudades
    double totalDistance;  // Distancia total
    double totalTime;      // Tiempo total
    double totalCost;      // Costo total (peajes)
    char routeType[20];    // "shortest", "fastest", "cheapest"
    time_t calculatedTime; // Momento del cálculo
    bool isValid;          // Si la ruta es válida
} Route;

// Entrada del caché de rutas
typedef struct CacheEntry {
    int fromCity;
    int toCity;
    Route* route;
    time_t timestamp;
    struct CacheEntry* next;
} CacheEntry;

// Caché de rutas
typedef struct {
    CacheEntry** buckets;
    int size;
    int capacity;
} RouteCache;

// Red de carreteras (grafo)
typedef struct {
    int** adjacencyMatrix;  // Matriz de adyacencia con pesos
    City* cities;          // Array de ciudades
    Road* roads;           // Array de carreteras
    int numCities;         // Número de ciudades
    int numRoads;          // Número de carreteras
    int capacity;          // Capacidad máxima
} RoadNetwork;

// Actualización de tráfico
typedef struct {
    int roadId;
    double trafficFactor;  // Multiplicador de tiempo (1.0 = normal, 2.0 = doble tiempo)
    time_t timestamp;
    int priority;          // Prioridad de la actualización
} TrafficUpdate;

// Sistema de navegación principal
typedef struct {
    RoadNetwork* network;
    HashMap* cityIndex;          // Búsqueda rápida de ciudades por nombre
    PriorityQueue* trafficQueue; // Cola de actualizaciones de tráfico
    RouteCache* routeCache;      // Caché de rutas calculadas
    time_t lastMaintenanceTime;  // Última limpieza del caché
    bool debugMode;              // Modo debug para logging
} NavigationSystem;

// =================================================================
// Prototipos de funciones principales
// =================================================================

// Inicialización del sistema
NavigationSystem* createNavigationSystem(int maxCities);
void destroyNavigationSystem(NavigationSystem* gps);
bool loadSystemFromFile(NavigationSystem* gps, const char* filename);
bool saveSystemToFile(NavigationSystem* gps, const char* filename);

// Gestión de ciudades
int addCity(NavigationSystem* gps, const char* name, double lat, double lon, int population, const char* region);
bool removeCity(NavigationSystem* gps, const char* name);
City* findCity(NavigationSystem* gps, const char* name);
void listCities(NavigationSystem* gps);

// Gestión de carreteras
bool addRoad(NavigationSystem* gps, const char* fromCity, const char* toCity,
             double distance, double time, double toll, const char* roadType, int speedLimit);
bool removeRoad(NavigationSystem* gps, const char* fromCity, const char* toCity);
void listRoads(NavigationSystem* gps, const char* cityName);

// Algoritmos de búsqueda de rutas
Route* findShortestPath(NavigationSystem* gps, const char* from, const char* to);
Route* findFastestPath(NavigationSystem* gps, const char* from, const char* to);
Route* findCheapestPath(NavigationSystem* gps, const char* from, const char* to);
Route** findAlternativeRoutes(NavigationSystem* gps, const char* from, const char* to, int maxRoutes, int* routeCount);

// Verificación de conectividad
bool isReachable(NavigationSystem* gps, const char* from, const char* to);
bool hasAlternativePath(NavigationSystem* gps, const char* from, const char* to, const char* avoidCity);
bool detectRouteLoops(NavigationSystem* gps);

// Gestión de tráfico
void updateTrafficConditions(NavigationSystem* gps, const char* fromCity, const char* toCity, double trafficFactor);
void processTrafficUpdates(NavigationSystem* gps);
void simulateTrafficCongestion(NavigationSystem* gps);
void clearTrafficConditions(NavigationSystem* gps);

// Caché de rutas
Route* getCachedRoute(NavigationSystem* gps, const char* from, const char* to, const char* routeType);
void cacheRoute(NavigationSystem* gps, const char* from, const char* to, Route* route);
void clearRouteCache(NavigationSystem* gps);
void performCacheMaintenance(NavigationSystem* gps);

// Análisis y estadísticas
void generateNetworkStatistics(NavigationSystem* gps);
void analyzeConnectivity(NavigationSystem* gps);
void findCriticalRoads(NavigationSystem* gps);
void identifyIsolatedCities(NavigationSystem* gps);

// Visualización y exportación
void exportNetworkToSVG(NavigationSystem* gps, const char* filename);
void exportNetworkToDOT(NavigationSystem* gps, const char* filename);
void visualizeRoute(NavigationSystem* gps, Route* route);
void printNetworkASCII(NavigationSystem* gps);

// Utilidades
double calculateHaversineDistance(GeoCoordinate p1, GeoCoordinate p2);
void freeRoute(Route* route);
void printRoute(NavigationSystem* gps, Route* route);
bool validateRoute(NavigationSystem* gps, Route* route);

void showMenu();
void runGPSDemo();

#endif //GPS_SYSTEM_H
