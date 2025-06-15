//
// Created by administrador on 5/27/25.
//

#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265359
#define INF INT_MAX

// Estructuras para visualización
typedef struct {
    double x, y;
} Point2D;

typedef struct {
    Point2D* positions;
    int numVertices;
    char** labels;
} GraphLayout;

// Prototipos
GraphLayout* createGraphLayout(int numVertices);
void freeGraphLayout(GraphLayout* layout);
void generateCircularLayout(GraphLayout* layout);
void generateRandomLayout(GraphLayout* layout, double width, double height);
void generateGridLayout(GraphLayout* layout, int cols, int rows);
void exportToSVG(const char* filename, int** graph, int numVertices, GraphLayout* layout);
void exportToDOT(const char* filename, int** graph, int numVertices, char** vertexNames, bool isDirected);
void exportToCSV(const char* filename, int** graph, int numVertices, char** vertexNames);
void printASCIIGraph(int** graph, int numVertices, char** vertexNames);
void printAdjacencyList(int** graph, int numVertices, char** vertexNames);
void visualizePathASCII(int* path, int pathLength, char** vertexNames);
void generateGraphStatistics(int** graph, int numVertices);

#endif //VISUALIZATION_H
