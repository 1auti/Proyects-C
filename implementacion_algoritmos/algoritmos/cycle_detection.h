//
// Created by administrador on 5/26/25.
//

#ifndef CYCLE_DETECTION_H
#define CYCLE_DETECTION_H

// Prototipos de funciones
bool hasCycleUndirected(int** graph, int numVertices);
bool hasCycleDirected(int** graph, int numVertices);
bool hasCycleUndirectedDFS(int** graph, int numVertices, int v, bool* visited, int parent);
bool hasCycleDFS(int** graph, int numVertices, int v, bool* visited, bool* recStack);
void printCycleDetectionResult(bool hasCycle, const char* graphType);

#endif //CYCLE_DETECTION_H
