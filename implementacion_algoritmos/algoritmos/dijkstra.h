//
// Created by administrador on 5/26/25.
//

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "common_types.h"

#define INF INT_MAX

// Prototipos de funciones
PathResult* dijkstra(int** graph, int numVertices, int start, int end);
int* dijkstraAllPaths(int** graph, int numVertices, int start);
void printDijkstraResult(PathResult* result, int start, int end);
void freePathResult(PathResult* result);

// Función auxiliar
int findMinDistance(int* dist, bool* visited, int numVertices);

#endif //DIJKSTRA_H
