#include "bellman_ford.h"

// Bellman-Ford para detectar ciclos negativos y encontrar caminos
PathResult* bellmanFord(int** graph, int numVertices, int start, int end) {
    PathResult* result = (PathResult*)malloc(sizeof(PathResult));
    result->path = NULL;
    result->pathLength = 0;
    result->totalWeight = INF;
    result->hasPath = false;
    
    if (start < 0 || start >= numVertices || end < 0 || end >= numVertices) {
        return result;
    }
    
    int* dist = (int*)malloc(numVertices * sizeof(int));
    int* parent = (int*)malloc(numVertices * sizeof(int));
    
    // Inicializar distancias
    for (int i = 0; i < numVertices; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;
    
    // Relajar todas las aristas V-1 veces
    for (int i = 0; i < numVertices - 1; i++) {
        bool updated = false;
        for (int u = 0; u < numVertices; u++) {
            for (int v = 0; v < numVertices; v++) {
                if (graph[u][v] != 0 && dist[u] != INF && 
                    dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                    parent[v] = u;
                    updated = true;
                }
            }
        }
        // Optimización: si no hay actualizaciones, salir temprano
        if (!updated) break;
    }
    
    // Verificar ciclos negativos
    bool hasNegativeCycle = false;
    for (int u = 0; u < numVertices; u++) {
        for (int v = 0; v < numVertices; v++) {
            if (graph[u][v] != 0 && dist[u] != INF && 
                dist[u] + graph[u][v] < dist[v]) {
                printf("¡Advertencia: Ciclo negativo detectado!\n");
                hasNegativeCycle = true;
                break;
            }
        }
        if (hasNegativeCycle) break;
    }
    
    // Construir el camino si existe y no hay ciclos negativos
    if (!hasNegativeCycle && dist[end] != INF) {
        result->hasPath = true;
        result->totalWeight = dist[end];
        
        // Contar longitud del camino
        int temp = end;
        int pathLen = 0;
        while (temp != -1) {
            pathLen++;
            temp = parent[temp];
        }
        
        // Crear array del camino
        result->path = (int*)malloc(pathLen * sizeof(int));
        result->pathLength = pathLen;
        
        temp = end;
        for (int i = pathLen - 1; i >= 0; i--) {
            result->path[i] = temp;
            temp = parent[temp];
        }
    }
    
    free(dist);
    free(parent);
    
    return result;
}

// Detectar todos los vértices afectados por ciclos negativos
bool* bellmanFordNegativeCycles(int** graph, int numVertices, int start) {
    bool* affectedByNegCycle = (bool*)calloc(numVertices, sizeof(bool));
    int* dist = (int*)malloc(numVertices * sizeof(int));
    
    // Inicializar
    for (int i = 0; i < numVertices; i++) {
        dist[i] = INF;
    }
    dist[start] = 0;
    
    // Algoritmo Bellman-Ford estándar
    for (int i = 0; i < numVertices - 1; i++) {
        for (int u = 0; u < numVertices; u++) {
            for (int v = 0; v < numVertices; v++) {
                if (graph[u][v] != 0 && dist[u] != INF && 
                    dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                }
            }
        }
    }
    
    // Encontrar vértices afectados por ciclos negativos
    for (int i = 0; i < numVertices; i++) {
        for (int u = 0; u < numVertices; u++) {
            for (int v = 0; v < numVertices; v++) {
                if (graph[u][v] != 0 && dist[u] != INF && 
                    dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = -INF;
                    affectedByNegCycle[v] = true;
                }
                
                if (dist[u] == -INF) {
                    affectedByNegCycle[u] = true;
                }
            }
        }
    }
    
    free(dist);
    return affectedByNegCycle;
}

// Función simple para detectar si existe algún ciclo negativo
bool bellmanFordDetectNegativeCycle(int** graph, int numVertices) {
    // Usar vértice 0 como punto de partida
    int* dist = (int*)malloc(numVertices * sizeof(int));
    
    // Inicializar distancias
    for (int i = 0; i < numVertices; i++) {
        dist[i] = INF;
    }
    dist[0] = 0;
    
    // Relajar todas las aristas V-1 veces
    for (int i = 0; i < numVertices - 1; i++) {
        for (int u = 0; u < numVertices; u++) {
            for (int v = 0; v < numVertices; v++) {
                if (graph[u][v] != 0 && dist[u] != INF && 
                    dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                }
            }
        }
    }
    
    // Verificar si se pueden relajar más aristas
    for (int u = 0; u < numVertices; u++) {
        for (int v = 0; v < numVertices; v++) {
            if (graph[u][v] != 0 && dist[u] != INF && 
                dist[u] + graph[u][v] < dist[v]) {
                free(dist);
                return true; // Ciclo negativo encontrado
            }
        }
    }
    
    free(dist);
    return false; // No hay ciclos negativos
}

void printBellmanFordResult(PathResult* result, int start, int end) {
    printf("\n=== RESULTADO BELLMAN-FORD ===\n");
    printf("Desde vértice %d hasta vértice %d:\n", start, end);
    
    if (!result->hasPath) {
        printf("No existe camino o hay ciclo negativo\n");
        return;
    }
    
    printf("Distancia: %d\n", result->totalWeight);
    printf("Camino: ");
    for (int i = 0; i < result->pathLength; i++) {
        printf("%d", result->path[i]);
        if (i < result->pathLength - 1) printf(" -> ");
    }
    printf("\n============================\n");
}