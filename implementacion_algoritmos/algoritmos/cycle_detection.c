#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cycle_detection.h"

// Detección de ciclos en grafo no dirigido usando DFS
bool hasCycleUndirectedDFS(int** graph, int numVertices, int v, bool* visited, int parent) {
    visited[v] = true;
    
    // Recorrer todos los vértices adyacentes
    for (int i = 0; i < numVertices; i++) {
        if (graph[v][i] != 0) { // Hay arista
            if (!visited[i]) {
                if (hasCycleUndirectedDFS(graph, numVertices, i, visited, v)) {
                    return true;
                }
            } else if (i != parent) {
                // Si el vértice ya está visitado y no es el padre, hay ciclo
                return true;
            }
        }
    }
    
    return false;
}

// Detección de ciclos en grafo no dirigido
bool hasCycleUndirected(int** graph, int numVertices) {
    bool* visited = (bool*)calloc(numVertices, sizeof(bool));
    
    // Verificar cada componente conexo
    for (int i = 0; i < numVertices; i++) {
        if (!visited[i]) {
            if (hasCycleUndirectedDFS(graph, numVertices, i, visited, -1)) {
                free(visited);
                return true;
            }
        }
    }
    
    free(visited);
    return false;
}

// Detección de ciclos en grafo dirigido usando DFS
bool hasCycleDFS(int** graph, int numVertices, int v, bool* visited, bool* recStack) {
    visited[v] = true;
    recStack[v] = true;
    
    // Recorrer todos los vértices adyacentes
    for (int i = 0; i < numVertices; i++) {
        if (graph[v][i] != 0) { // Hay arista dirigida de v a i
            if (!visited[i]) {
                if (hasCycleDFS(graph, numVertices, i, visited, recStack)) {
                    return true;
                }
            } else if (recStack[i]) {
                // Si el vértice está en la pila de recursión, hay ciclo
                return true;
            }
        }
    }
    
    recStack[v] = false; // Remover de la pila de recursión
    return false;
}

// Detección de ciclos en grafo dirigido
bool hasCycleDirected(int** graph, int numVertices) {
    bool* visited = (bool*)calloc(numVertices, sizeof(bool));
    bool* recStack = (bool*)calloc(numVertices, sizeof(bool));
    
    // Verificar cada vértice
    for (int i = 0; i < numVertices; i++) {
        if (!visited[i]) {
            if (hasCycleDFS(graph, numVertices, i, visited, recStack)) {
                free(visited);
                free(recStack);
                return true;
            }
        }
    }
    
    free(visited);
    free(recStack);
    return false;
}

// Función para detectar ciclos automáticamente (determina si es dirigido o no)
bool detectCycles(int** graph, int numVertices, bool isDirected) {
    if (isDirected) {
        return hasCycleDirected(graph, numVertices);
    } else {
        return hasCycleUndirected(graph, numVertices);
    }
}

void printCycleDetectionResult(bool hasCycle, const char* graphType) {
    printf("\n=== DETECCIÓN DE CICLOS ===\n");
    printf("Tipo de grafo: %s\n", graphType);
    printf("¿Tiene ciclos? %s\n", hasCycle ? "SÍ" : "NO");
    printf("========================\n");
}