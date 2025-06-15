//
// Created by administrador on 5/27/25.
//

#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Estructuras para manejo de archivos
typedef struct {
    int** matrix;
    int numVertices;
    bool isDirected;
    char** vertexNames;
} GraphData;

// Prototipos
GraphData* loadGraphFromFile(const char* filename);
bool saveGraphToFile(const char* filename, int** graph, int numVertices, bool isDirected, char** vertexNames);
bool loadAdjacencyMatrix(const char* filename, int*** graph, int* numVertices);
bool saveAdjacencyMatrix(const char* filename, int** graph, int numVertices);
bool loadEdgeList(const char* filename, int*** graph, int* numVertices);
bool saveEdgeList(const char* filename, int** graph, int numVertices);
void freeGraphData(GraphData* data);

#endif //FILE_IO_H
