#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100

typedef struct Graph {
    int numVertices;
    int adjMatrix[MAX_VERTICES][MAX_VERTICES];
} Graph;

// Función para crear un grafo con n vértices
Graph* createGraph(int n) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        printf("Error: No se pudo asignar memoria para el grafo\n");
        return NULL;
    }

    graph->numVertices = n;

    // Inicializar matriz con ceros (sin aristas)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph->adjMatrix[i][j] = 0;
        }
    }

    return graph;
}

// Función para añadir una arista entre dos vértices
void addEdge(Graph* graph, int src, int dest) {
    if (src >= 0 && src < graph->numVertices &&
        dest >= 0 && dest < graph->numVertices) {
        graph->adjMatrix[src][dest] = 1;

        // Para grafo no dirigido, descomenta la siguiente línea
        // graph->adjMatrix[dest][src] = 1;
        }
}

// Función para imprimir el grafo
void printGraph(Graph* graph) {
    printf("Matriz de adyacencia:\n");
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->numVertices; j++) {
            printf("%d ", graph->adjMatrix[i][j]);
        }
        printf("\n");
    }
}

// Función para liberar la memoria del grafo
void freeGraph(Graph* graph) {
    free(graph);
}

// Ejemplo de uso
int main() {
    Graph* graph = createGraph(5);

    addEdge(graph, 0, 1);
    addEdge(graph, 0, 4);
    addEdge(graph, 1, 2);
    addEdge(graph, 1, 3);
    addEdge(graph, 1, 4);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 4);

    printGraph(graph);

    freeGraph(graph);
    return 0;
}