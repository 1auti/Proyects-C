#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Estructura Union-Find (Disjoint Set Union)
typedef struct {
    int* parent;
    int* rank;
    int* size;
    int numSets;
    int totalElements;
} UnionFind;

// Prototipos de funciones principales
UnionFind* createUnionFind(int n);
void destroyUnionFind(UnionFind* uf);

// Operaciones básicas
int find(UnionFind* uf, int x);
bool unionByRank(UnionFind* uf, int x, int y);
bool unionBySize(UnionFind* uf, int x, int y);
bool areConnected(UnionFind* uf, int x, int y);

// Utilidades
int getSetSize(UnionFind* uf, int x);
int getNumSets(UnionFind* uf);
void printUnionFind(UnionFind* uf);
int* getSetMembers(UnionFind* uf, int x, int* setSize);

// Funciones auxiliares
void makeSet(UnionFind* uf, int x);
int findWithoutCompression(UnionFind* uf, int x);

#endif