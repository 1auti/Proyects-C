#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * implementación del algoritmo de Búsqueda en Profundidad (DFS) utilizando la lista de adyacencia
 */

// Estructura para el nodo de la lista enlazada
typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

// Estructura para la lista de adyacencia
typedef struct AdjList {
    AdjListNode* head;
} AdjList;

// Estructura para el grafo
typedef struct Graph {
    int numVertices;
    AdjList* array;
} Graph;

// Función para crear un nuevo nodo de la lista de adyacencia
AdjListNode* createNode(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (!newNode) {
        printf("Error: No se pudo asignar memoria para el nodo\n");
        return NULL;
    }

    newNode->dest = dest;
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

// Función para añadir una arista entre dos vértices
void addEdge(Graph* graph, int src, int dest) {
    if (src < 0 || src >= graph->numVertices ||
        dest < 0 || dest >= graph->numVertices) {
        return;
    }

    // Añadir una arista de src a dest
    AdjListNode* newNode = createNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Para grafo no dirigido, añadir también de dest a src
    // Descomenta el siguiente bloque si es un grafo no dirigido
    /*
    newNode = createNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
    */
}

// Función para imprimir el grafo
void printGraph(Graph* graph) {
    printf("Listas de adyacencia:\n");
    for (int v = 0; v < graph->numVertices; v++) {
        AdjListNode* current = graph->array[v].head;
        printf("Vértice %d: ", v);

        while (current) {
            printf("-> %d ", current->dest);
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


// Función para realizar DFS
void DFSUtil(Graph* graph, int vertex, bool visited[]) {
    // Marcar el vértice actual como visitado e imprimirlo
    visited[vertex] = true;
    printf("%d ", vertex);
    
    // Recorrer todos los vértices adyacentes
    AdjListNode* adjNode = graph->array[vertex].head;
    while (adjNode) {
        int adjVertex = adjNode->dest;
        if (!visited[adjVertex]) {
            DFSUtil(graph, adjVertex, visited);
        }
        adjNode = adjNode->next;
    }
}

// Función para iniciar DFS desde un vértice específico
void DFS(Graph* graph, int startVertex) {
    // Crear arreglo para seguimiento de vértices visitados
    bool* visited = (bool*)malloc(graph->numVertices * sizeof(bool));
    if (!visited) {
        printf("Error: No se pudo asignar memoria para el arreglo visited\n");
        return;
    }
    
    // Inicializar todos los vértices como no visitados
    for (int i = 0; i < graph->numVertices; i++) {
        visited[i] = false;
    }
    
    printf("Recorrido DFS (empezando desde vértice %d): ", startVertex);
    DFSUtil(graph, startVertex, visited);
    printf("\n");
    
    free(visited);
}

// Función para realizar BFS
void BFS(Graph* graph, int startVertex) {
    // Crear arreglo para seguimiento de vértices visitados
    bool* visited = (bool*)malloc(graph->numVertices * sizeof(bool));
    if (!visited) {
        printf("Error: No se pudo asignar memoria para el arreglo visited\n");
        return;
    }
    
    // Inicializar todos los vértices como no visitados
    for (int i = 0; i < graph->numVertices; i++) {
        visited[i] = false;
    }
    
    // Crear una cola para BFS
    int* queue = (int*)malloc(graph->numVertices * sizeof(int));
    if (!queue) {
        printf("Error: No se pudo asignar memoria para la cola\n");
        free(visited);
        return;
    }
    
    int front = 0, rear = 0;
    
    // Marcar el vértice actual como visitado y encolarlo
    visited[startVertex] = true;
    queue[rear++] = startVertex;
    
    printf("Recorrido BFS (empezando desde vértice %d): ", startVertex);
    
    // El proceso de BFS
    while (front < rear) {
        // Desencolar un vértice y mostrarlo
        int currentVertex = queue[front++];
        printf("%d ", currentVertex);
        
        // Obtener todos los vértices adyacentes al vértice desencolado
        AdjListNode* adjNode = graph->array[currentVertex].head;
        
        // Si un adyacente no ha sido visitado, marcarlo como visitado y encolarlo
        while (adjNode) {
            int adjVertex = adjNode->dest;
            if (!visited[adjVertex]) {
                visited[adjVertex] = true;
                queue[rear++] = adjVertex;
            }
            adjNode = adjNode->next;
        }
    }
    
    printf("\n");
    
    free(queue);
    free(visited);
}

// Ejemplo de uso ampliado
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
    
    DFS(graph, 0);
    BFS(graph, 0);
    
    freeGraph(graph);
    return 0;
}