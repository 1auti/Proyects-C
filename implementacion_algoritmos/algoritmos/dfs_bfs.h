#ifndef DFS_BFS_H
#define DFS_BFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Estructura para resultado de búsqueda
typedef struct {
    int* visitOrder;
    int visitCount;
    bool* visited;
    int* distances;
    int* parent;
} SearchResult;

// Prototipos de funciones DFS
SearchResult* dfs(int** graph, int numVertices, int startVertex);
void dfsRecursive(int** graph, int numVertices, int vertex, bool* visited, int* visitOrder, int* count);
void dfsIterative(int** graph, int numVertices, int startVertex, bool* visited, int* visitOrder, int* count);

// Prototipos de funciones BFS
SearchResult* bfs(int** graph, int numVertices, int startVertex);
void bfsTraversal(int** graph, int numVertices, int startVertex, bool* visited, int* visitOrder, int* count, int* distances, int* parent);

// Funciones de conectividad
bool isConnected(int** graph, int numVertices, int vertex1, int vertex2);
int* getConnectedComponents(int** graph, int numVertices, int* numComponents);
bool isGraphConnected(int** graph, int numVertices);

// Funciones de utilidad
void printSearchResult(SearchResult* result, const char* algorithm, int startVertex);
void freeSearchResult(SearchResult* result);
void printPath(int* parent, int start, int end);

#endif