#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_types.h"
#include "../estructura_datos/union_find.h"

// Estructura para Árbol de Expansión Mínima
typedef struct {
    Edge* edges;
    int edgeCount;
    int totalWeight;
} MST;

// Prototipos de funciones principales
MST* kruskalMST(int** graph, int numVertices);
void printMST(MST* mst);
void freeMST(MST* mst);

// Función auxiliar
int compareEdges(const void* a, const void* b);

#endif