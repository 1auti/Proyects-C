//
// Created by administrador on 5/26/25.
//

#include "dijkstra.h"

// Funciones para encontrar el vertice con distancia minima no visitado
int findMinDistance(int * dist, bool * visited,int numVertices) {
    int min = INF, minIndex = -1;

    for(int i = 0 ; i < numVertices; i++) {
        if(!visited[i] && dist[i] <= min) {
            min = dist[i];
            minIndex = i;
        }
    }
    return minIndex;
}

// Para el camino mas corto entre dos vertices especificos
PathResult * dijkstra(int ** graph, int numVertices, int inicio, int fin) {
    PathResult * result = (PathResult*)malloc(sizeof(PathResult));
    result->path = NULL;
    result->pathLength = 0 ;
    result->totalWeigth = INF;
    result->hasPath = false;

    // Si entra en la validacion termina el codigo
    if(inicio < 0 || inicio >= numVertices || fin < 0 || fin >= numVertices) {
        return  result;
    }

    int * dist = (int*)malloc(numVertices * sizeof(int));
    bool * visited = (bool*)calloc(numVertices,sizeof(bool));
    int * parent = (int*)malloc(numVertices * sizeof(int));

    // Inicializar distancias y padres
    for(int i = 0 ; i < numVertices; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[inicio] = 0;


    // Algoritmo principal de Dijkstra
    for (int count = 0; count < numVertices - 1; count++) {
        int u = findMinDistance(dist, visited, numVertices);

        if (u == -1) break; // No hay más vértices alcanzables

        visited[u] = true;

        // Si llegamos al destino, podemos parar
        if (u == fin) break;

        // Actualizar distancias de vértices adyacentes
        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && graph[u][v] != 0 &&
                dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    // Construir el camino si existe
    if (dist[fin] != INF) {
        result->hasPath = true;
        result->totalWeigth = dist[fin];

        // Contar la longitud del camino
        int temp = fin;
        int pathLen = 0;
        while (temp != -1) {
            pathLen++;
            temp = parent[temp];
        }

        // Crear el array del camino
        result->path = (int*)malloc(pathLen * sizeof(int));
        result->pathLength = pathLen;

        temp = fin;
        for (int i = pathLen - 1; i >= 0; i--) {
            result->path[i] = temp;
            temp = parent[temp];
        }
    }

    free(dist);
    free(visited);
    free(parent);

    return result;
}

// Dijkstra para todos los caminos desde un vértice
int* dijkstraAllPaths(int** graph, int numVertices, int start) {
    int* dist = (int*)malloc(numVertices * sizeof(int));
    bool* visited = (bool*)calloc(numVertices, sizeof(bool));

    // Inicializar distancias
    for (int i = 0; i < numVertices; i++) {
        dist[i] = INF;
    }
    dist[start] = 0;

    for (int count = 0; count < numVertices - 1; count++) {
        int u = findMinDistance(dist, visited, numVertices);

        if (u == -1) break;

        visited[u] = true;

        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && graph[u][v] != 0 &&
                dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    free(visited);
    return dist;
}

void printDijkstraResult(PathResult* result, int start, int end) {
    printf("\n=== RESULTADO DIJKSTRA ===\n");
    printf("Desde vértice %d hasta vértice %d:\n", start, end);

    if (!result->hasPath) {
        printf("No existe camino\n");
        return;
    }

    printf("Distancia mínima: %d\n", result->totalWeigth);
    printf("Camino: ");
    for (int i = 0; i < result->pathLength; i++) {
        printf("%d", result->path[i]);
        if (i < result->pathLength - 1) printf(" -> ");
    }
    printf("\n========================\n");
}

void freePathResult(PathResult* result) {
    if (result) {
        if (result->path) {
            free(result->path);
        }
        free(result);
    }
}



