#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Estructura para arista
typedef struct {
    int src, dest;
    int weight;
} Edge;

// Estructura para Árbol de Expansión Mínima
typedef struct {
    Edge* edges;
    int edgeCount;
    int totalWeight;
} MST;

// Estructura para Union-Find
typedef struct {
    int* parent;
    int* rank;
    int size;
} UnionFind;

// Prototipos de funciones principales
MST* kruskalMST(int** graph, int numVertices);
void printMST(MST* mst);
void freeMST(MST* mst);

// Prototipos para Union-Find
UnionFind* createUnionFind(int size);
int findSet(UnionFind* uf, int x);
void unionSets(UnionFind* uf, int x, int y);
void freeUnionFind(UnionFind* uf);

// Función auxiliar
int compareEdges(const void* a, const void* b);

#endif