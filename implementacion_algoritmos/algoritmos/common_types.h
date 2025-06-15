//
// Created by administrador on 5/26/25.
//

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define INF INT_MAX

// Estructura para resultado de camino (COMPARTIDA entre algoritmos)
typedef struct {
    int* path;
    int pathLength;
    int totalWeight;
    bool hasPath;
} PathResult;


// Estructura para representar una arista
typedef struct {
    int src, dest;
    int weight;
} Edge;

// Funciones comunes
void freePathResult(PathResult* result);

#endif //COMMON_TYPES_H