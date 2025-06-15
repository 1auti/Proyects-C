// ============================================================================
// core/route.h - Header para gestión de rutas
// ============================================================================
#ifndef ROUTE_H
#define ROUTE_H

#include "../utils/time_utils.h"
#include "station.h"
#include "line.h"
#include <stdbool.h>

// ============================================================================
// ESTRUCTURA PRINCIPAL DE RUTA
// ============================================================================

// Para calcular la ruta
typedef struct Route {
    Station** path;              // Camino a la estacion
    int path_length;             // Numero de estaciones en el camino
    Line** lines_used;           // Lineas utilizadas
    int transfer_count;          // Numero de transbordos
    double total_cost;           // Costo total
    int total_time_minutes;      // Tiempo total de minutos
    DateTime* departure_time;    // Hora de salida
    DateTime* arrival_time;      // Hora de llegada
    int capacity_path;           // Capacidad del array de camino
    int capacity_lines;          // Capacidad del array de lineas
} Route;

// ============================================================================
// FUNCIONES DE CREACIÓN Y DESTRUCCIÓN
// ============================================================================

/**
 * @brief Crea una nueva ruta con capacidad inicial
 * @param initial_capacity Capacidad inicial para estaciones y líneas
 * @return Puntero a la nueva ruta o NULL si falla
 */
Route* createRoute(int initial_capacity);

/**
 * @brief Destruye una ruta y libera toda su memoria
 * @param route Ruta a destruir
 */
void destroyRoute(Route* route);

/**
 * @brief Crea una copia exacta de una ruta
 * @param original Ruta original a copiar
 * @return Puntero a la nueva ruta clonada o NULL si falla
 */
Route* cloneRoute(Route* original);

// ============================================================================
// GESTIÓN DE ESTACIONES EN LA RUTA
// ============================================================================

/**
 * @brief Agrega una estación a la ruta (detecta transbordos automáticamente)
 * @param route Ruta donde agregar
 * @param station Estación a agregar
 * @param line Línea utilizada para llegar a esta estación
 * @return true si se agregó exitosamente, false si falló
 */
bool addStationToRoute(Route* route, Station* station, Line* line);

/**
 * @brief Remueve la última estación de la ruta
 * @param route Ruta de donde remover
 * @return true si se removió exitosamente, false si falló
 */
bool removeLastStationFromRoute(Route* route);

/**
 * @brief Obtiene la estación en una posición específica
 * @param route Ruta a consultar
 * @param position Posición de la estación (0-indexado)
 * @return Puntero a la estación o NULL si posición inválida
 */
Station* getStationAtPosition(Route* route, int position);

/**
 * @brief Obtiene la línea utilizada en una posición específica
 * @param route Ruta a consultar
 * @param position Posición de la línea (0-indexado)
 * @return Puntero a la línea o NULL si posición inválida
 */
Line* getLineAtPosition(Route* route, int position);

/**
 * @brief Verifica si una estación está presente en la ruta
 * @param route Ruta a verificar
 * @param station Estación a buscar
 * @return true si la estación está en la ruta, false si no
 */
bool isStationInRoute(Route* route, Station* station);

/**
 * @brief Encuentra la posición de una estación en la ruta
 * @param route Ruta a buscar
 * @param station Estación a localizar
 * @return Posición de la estación o -1 si no se encuentra
 */
int getStationPositionInRoute(Route* route, Station* station);

/**
 * @brief Obtiene la estación de origen de la ruta
 * @param route Ruta a consultar
 * @return Puntero a la estación de origen o NULL si ruta vacía
 */
Station* getOriginStation(Route* route);

/**
 * @brief Obtiene la estación de destino de la ruta
 * @param route Ruta a consultar
 * @return Puntero a la estación de destino o NULL si ruta vacía
 */
Station* getDestinationStation(Route* route);

// ============================================================================
// GESTIÓN DE TIEMPOS DateTime
// ============================================================================

/**
 * @brief Establece la hora de salida de la ruta
 * @param route Ruta a modificar
 * @param departure Tiempo de salida
 * @return true si se estableció exitosamente, false si falló
 */
bool setDepartureTime(Route* route, DateTime departure);

/**
 * @brief Establece la hora de llegada de la ruta
 * @param route Ruta a modificar
 * @param arrival Tiempo de llegada
 * @return true si se estableció exitosamente, false si falló
 */
bool setArrivalTime(Route* route, DateTime arrival);

/**
 * @brief Calcula automáticamente la hora de llegada basada en salida y duración
 * @param route Ruta con tiempo de salida y duración establecidos
 * @return true si se calculó exitosamente, false si falló
 */
bool calculateArrivalTime(Route* route);

// ============================================================================
// ANÁLISIS Y CONSULTAS DE RUTA
// ============================================================================

/**
 * @brief Obtiene el número de estaciones en la ruta
 * @param route Ruta a consultar
 * @return Número de estaciones
 */
int getRouteLength(Route* route);

/**
 * @brief Obtiene el costo total de la ruta
 * @param route Ruta a consultar
 * @return Costo total
 */
double getRouteCost(Route* route);

/**
 * @brief Obtiene el tiempo total de viaje en minutos
 * @param route Ruta a consultar
 * @return Tiempo total en minutos
 */
int getRouteTotalTime(Route* route);

/**
 * @brief Obtiene el número de transbordos en la ruta
 * @param route Ruta a consultar
 * @return Número de transbordos
 */
int getRouteTransferCount(Route* route);

/**
 * @brief Calcula la eficiencia de la ruta
 * @param route Ruta a evaluar
 * @return Índice de eficiencia (mayor = más eficiente)
 */
double calculateRouteEfficiency(Route* route);

// ============================================================================
// VALIDACIÓN DE RUTAS
// ============================================================================

/**
 * @brief Valida que la ruta sea coherente y completa
 * @param route Ruta a validar
 * @return true si la ruta es válida, false si no
 */
bool isValidRoute(Route* route);

/**
 * @brief Verifica si la ruta es circular (origen = destino)
 * @param route Ruta a verificar
 * @return true si es circular, false si no
 */
bool hasCircularPath(Route* route);

/**
 * @brief Verifica si la ruta pasa por una estación específica
 * @param route Ruta a verificar
 * @param station_id ID de la estación a buscar
 * @return true si pasa por la estación, false si no
 */
bool routePassesThroughStation(Route* route, int station_id);

// ============================================================================
// OPTIMIZACIÓN DE RUTAS
// ============================================================================

/**
 * @brief Optimiza la ruta eliminando duplicados y redundancias
 * @param route Ruta a optimizar
 * @return true si se realizaron optimizaciones, false si no
 */
bool optimizeRoute(Route* route);

/**
 * @brief Invierte el orden de la ruta (origen ↔ destino)
 * @param route Ruta a invertir
 * @return true si se invirtió exitosamente, false si falló
 */
bool reverseRoute(Route* route);

// ============================================================================
// VISUALIZACIÓN DE RUTAS
// ============================================================================

/**
 * @brief Imprime la ruta completa con detalles
 * @param route Ruta a imprimir
 */
void printRoute(Route* route);

/**
 * @brief Imprime un resumen compacto de la ruta en una línea
 * @param route Ruta a imprimir
 */
void printRouteCompact(Route* route);

/**
 * @brief Imprime un resumen estadístico de la ruta
 * @param route Ruta a imprimir
 */
void printRouteSummary(Route* route);

/**
 * @brief Imprime estadísticas detalladas de la ruta
 * @param route Ruta a analizar
 */
void printRouteStatistics(Route* route);

// ============================================================================
// COMPARACIÓN DE RUTAS
// ============================================================================

/**
 * @brief Función de comparación por tiempo (para qsort)
 * @param a Puntero a Route*
 * @param b Puntero a Route*
 * @return Comparación de tiempos
 */
int compareRoutesByTime(const void* a, const void* b);

/**
 * @brief Función de comparación por costo (para qsort)
 * @param a Puntero a Route*
 * @param b Puntero a Route*
 * @return Comparación de costos
 */
int compareRoutesByCost(const void* a, const void* b);

/**
 * @brief Función de comparación por transbordos (para qsort)
 * @param a Puntero a Route*
 * @param b Puntero a Route*
 * @return Comparación de transbordos
 */
int compareRoutesByTransfers(const void* a, const void* b);

// ============================================================================
// UTILIDADES PARA ARRAYS DE RUTAS
// ============================================================================

/**
 * @brief Imprime múltiples opciones de ruta
 * @param routes Array de rutas
 * @param route_count Número de rutas en el array
 */
void printRouteOptions(Route** routes, int route_count);

/**
 * @brief Libera un array de rutas y toda su memoria
 * @param routes Array de rutas a liberar
 * @param route_count Número de rutas en el array
 */
void freeRouteArray(Route** routes, int route_count);

/**
 * @brief Ordena un array de rutas por tiempo
 * @param routes Array de rutas a ordenar
 * @param route_count Número de rutas
 */
void sortRoutesByTime(Route** routes, int route_count);

/**
 * @brief Ordena un array de rutas por costo
 * @param routes Array de rutas a ordenar
 * @param route_count Número de rutas
 */
void sortRoutesByCost(Route** routes, int route_count);

/**
 * @brief Ordena un array de rutas por transbordos
 * @param routes Array de rutas a ordenar
 * @param route_count Número de rutas
 */
void sortRoutesByTransfers(Route** routes, int route_count);

// ============================================================================
// CONSTANTES Y MACROS ÚTILES
// ============================================================================

#define MAX_ROUTE_STATIONS 50        // Máximo de estaciones por ruta
#define MAX_TRANSFERS 5              // Máximo de transbordos recomendado
#define INVALID_POSITION -1          // Valor para posición inválida

// Macros para facilitar el uso
#define ROUTE_IS_EMPTY(route) ((route) == NULL || (route)->path_length == 0)
#define ROUTE_HAS_TRANSFERS(route) ((route) != NULL && (route)->transfer_count > 0)
#define ROUTE_IS_DIRECT(route) ((route) != NULL && (route)->transfer_count == 0)

#endif // ROUTE_H