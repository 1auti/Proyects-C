#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define INF INT_MAX

// Estructura para resultado de camino (reutilizamos la de dijkstra)
typedef struct {
    int* path;
    int pathLength;
    int totalWeight;
    bool hasPath;
} PathResult;

// Prototipos de funciones
PathResult* bellmanFord(int** graph, int numVertices, int start, int end);
bool* bellmanFordNegativeCycles(int** graph, int numVertices, int start);
bool bellmanFordDetectNegativeCycle(int** graph, int numVertices);
void printBellmanFordResult(PathResult* result, int start, int end);

#endif