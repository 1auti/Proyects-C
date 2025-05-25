#include <stdio.h>
#include <stdlib.h>

// Estructura para el nodo de la lista enlazada con peso
typedef struct AdjListNode {
    int dest;
    int weight;
    struct AdjListNode* next;
} AdjListNode;

// Estructura para la lista de adyacencia
typedef struct AdjList {
    AdjListNode* head;
} AdjList;

// Estructura para el grafo ponderado
typedef struct Graph {
    int numVertices;
    AdjList* array;
} Graph;

// Función para crear un nuevo nodo de la lista de adyacencia
AdjListNode* createNode(int dest, int weight) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (!newNode) {
        printf("Error: No se pudo asignar memoria para el nodo\n");
        return NULL;
    }

    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// Función para crear un grafo con n vértices
Graph* createGraph(int n) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        printf("Error: No se pudo asignar memoria para el grafo\n");
        return NULL;
    }

    graph->numVertices = n;

    // Crear un arreglo de listas de adyacencia
    graph->array = (AdjList*)malloc(n * sizeof(AdjList));
    if (!graph->array) {
        printf("Error: No se pudo asignar memoria para las listas\n");
        free(graph);
        return NULL;
    }

    // Inicializar cada lista de adyacencia como vacía
    for (int i = 0; i < n; i++) {
        graph->array[i].head = NULL;
    }

    return graph;
}

// Función para añadir una arista con peso entre dos vértices
void addEdge(Graph* graph, int src, int dest, int weight) {
    if (src < 0 || src >= graph->numVertices ||
        dest < 0 || dest >= graph->numVertices) {
        return;
    }

    // Añadir una arista de src a dest
    AdjListNode* newNode = createNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Para grafo no dirigido, añadir también de dest a src
    // Descomenta el siguiente bloque si es un grafo no dirigido
    /*
    newNode = createNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
    */
}

// Función para imprimir el grafo ponderado
void printGraph(Graph* graph) {
    printf("Listas de adyacencia del grafo ponderado:\n");
    for (int v = 0; v < graph->numVertices; v++) {
        AdjListNode* current = graph->array[v].head;
        printf("Vértice %d: ", v);

        while (current) {
            printf("-> %d(peso: %d) ", current->dest, current->weight);
            current = current->next;
        }
        printf("\n");
    }
}

// Función para liberar la memoria del grafo
void freeGraph(Graph* graph) {
    if (graph) {
        // Liberar todas las listas de adyacencia
        for (int v = 0; v < graph->numVertices; v++) {
            AdjListNode* current = graph->array[v].head;
            while (current) {
                AdjListNode* temp = current;
                current = current->next;
                free(temp);
            }
        }

        free(graph->array);
        free(graph);
    }
}

// Ejemplo de uso
int main() {
    Graph* graph = createGraph(5);

    // Añadir aristas con pesos
    addEdge(graph, 0, 1, 10);
    addEdge(graph, 0, 4, 5);
    addEdge(graph, 1, 2, 3);
    addEdge(graph, 1, 3, 7);
    addEdge(graph, 1, 4, 2);
    addEdge(graph, 2, 3, 8);
    addEdge(graph, 3, 4, 4);

    printGraph(graph);

    freeGraph(graph);
    return 0;
}