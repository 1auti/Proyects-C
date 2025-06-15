#include "dfs_bfs.h"

// Crear estructura de resultado
SearchResult* createSearchResult(int numVertices) {
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    result->visitOrder = (int*)malloc(numVertices * sizeof(int));
    result->visitCount = 0;
    result->visited = (bool*)calloc(numVertices, sizeof(bool));
    result->distances = (int*)malloc(numVertices * sizeof(int));
    result->parent = (int*)malloc(numVertices * sizeof(int));
    
    // Inicializar distancias y padres
    for (int i = 0; i < numVertices; i++) {
        result->distances[i] = -1; // -1 indica no visitado
        result->parent[i] = -1;
    }
    
    return result;
}

// DFS recursivo (función auxiliar)
void dfsRecursive(int** graph, int numVertices, int vertex, bool* visited, int* visitOrder, int* count) {
    visited[vertex] = true;
    visitOrder[(*count)++] = vertex;
    
    // Visitar todos los vértices adyacentes no visitados
    for (int i = 0; i < numVertices; i++) {
        if (graph[vertex][i] != 0 && !visited[i]) {
            dfsRecursive(graph, numVertices, i, visited, visitOrder, count);
        }
    }
}

// DFS iterativo usando stack
void dfsIterative(int** graph, int numVertices, int startVertex, bool* visited, int* visitOrder, int* count) {
    int* stack = (int*)malloc(numVertices * sizeof(int));
    int top = 0;
    
    stack[top++] = startVertex;
    
    while (top > 0) {
        int vertex = stack[--top];
        
        if (!visited[vertex]) {
            visited[vertex] = true;
            visitOrder[(*count)++] = vertex;
            
            // Agregar vértices adyacentes al stack (en orden reverso para mantener orden)
            for (int i = numVertices - 1; i >= 0; i--) {
                if (graph[vertex][i] != 0 && !visited[i]) {
                    stack[top++] = i;
                }
            }
        }
    }
    
    free(stack);
}

// Función principal DFS
SearchResult* dfs(int** graph, int numVertices, int startVertex) {
    if (startVertex < 0 || startVertex >= numVertices) {
        return NULL;
    }
    
    SearchResult* result = createSearchResult(numVertices);
    
    // Usar DFS recursivo por defecto
    dfsRecursive(graph, numVertices, startVertex, result->visited, result->visitOrder, &result->visitCount);
    
    return result;
}

// BFS usando cola
void bfsTraversal(int** graph, int numVertices, int startVertex, bool* visited, int* visitOrder, int* count, int* distances, int* parent) {
    int* queue = (int*)malloc(numVertices * sizeof(int));
    int front = 0, rear = 0;
    
    visited[startVertex] = true;
    distances[startVertex] = 0;
    visitOrder[(*count)++] = startVertex;
    queue[rear++] = startVertex;
    
    while (front < rear) {
        int vertex = queue[front++];
        
        // Procesar todos los vértices adyacentes
        for (int i = 0; i < numVertices; i++) {
            if (graph[vertex][i] != 0 && !visited[i]) {
                visited[i] = true;
                distances[i] = distances[vertex] + 1;
                parent[i] = vertex;
                visitOrder[(*count)++] = i;
                queue[rear++] = i;
            }
        }
    }
    
    free(queue);
}

// Función principal BFS
SearchResult* bfs(int** graph, int numVertices, int startVertex) {
    if (startVertex < 0 || startVertex >= numVertices) {
        return NULL;
    }
    
    SearchResult* result = createSearchResult(numVertices);
    
    bfsTraversal(graph, numVertices, startVertex, result->visited, 
                 result->visitOrder, &result->visitCount, result->distances, result->parent);
    
    return result;
}

// Verificar si dos vértices están conectados
bool isConnected(int** graph, int numVertices, int vertex1, int vertex2) {
    if (vertex1 < 0 || vertex1 >= numVertices || vertex2 < 0 || vertex2 >= numVertices) {
        return false;
    }
    
    if (vertex1 == vertex2) return true;
    
    SearchResult* result = bfs(graph, numVertices, vertex1);
    bool connected = result->visited[vertex2];
    
    freeSearchResult(result);
    return connected;
}

// Encontrar componentes conexos
int* getConnectedComponents(int** graph, int numVertices, int* numComponents) {
    int* components = (int*)malloc(numVertices * sizeof(int));
    bool* visited = (bool*)calloc(numVertices, sizeof(bool));
    *numComponents = 0;
    
    for (int i = 0; i < numVertices; i++) {
        components[i] = -1;
    }
    
    for (int i = 0; i < numVertices; i++) {
        if (!visited[i]) {
            // Nuevo componente encontrado
            int currentComponent = *numComponents;
            (*numComponents)++;
            
            // DFS para marcar todos los vértices del componente
            int* stack = (int*)malloc(numVertices * sizeof(int));
            int top = 0;
            
            stack[top++] = i;
            
            while (top > 0) {
                int vertex = stack[--top];
                
                if (!visited[vertex]) {
                    visited[vertex] = true;
                    components[vertex] = currentComponent;
                    
                    // Agregar vértices adyacentes
                    for (int j = 0; j < numVertices; j++) {
                        if (graph[vertex][j] != 0 && !visited[j]) {
                            stack[top++] = j;
                        }
                    }
                }
            }
            
            free(stack);
        }
    }
    
    free(visited);
    return components;
}

// Verificar si el grafo es conexo
bool isGraphFullyConnected(int** graph, int numVertices) {
    if (numVertices <= 1) return true;
    
    SearchResult* result = dfs(graph, numVertices, 0);
    bool connected = (result->visitCount == numVertices);
    
    freeSearchResult(result);
    return connected;
}

void printSearchResult(SearchResult* result, const char* algorithm, int startVertex) {
    printf("\n=== RESULTADO %s ===\n", algorithm);
    printf("Vértice inicial: %d\n", startVertex);
    printf("Orden de visita: ");
    
    for (int i = 0; i < result->visitCount; i++) {
        printf("%d", result->visitOrder[i]);
        if (i < result->visitCount - 1) printf(" -> ");
    }
    
    printf("\nVértices visitados: %d\n", result->visitCount);
    
    if (result->distances[0] != -1) {
        printf("Distancias desde vértice %d:\n", startVertex);
        for (int i = 0; i < result->visitCount; i++) {
            int vertex = result->visitOrder[i];
            if (result->distances[vertex] != -1) {
                printf("  Vértice %d: distancia %d\n", vertex, result->distances[vertex]);
            }
        }
    }
    
    printf("=========================\n");
}

void printPath(int* parent, int start, int end) {
    if (parent[end] == -1 && start != end) {
        printf("No hay camino de %d a %d\n", start, end);
        return;
    }
    
    // Construir el camino
    int* path = (int*)malloc(100 * sizeof(int)); // Asumimos máximo 100 vértices en el camino
    int pathLength = 0;
    int current = end;
    
    while (current != -1) {
        path[pathLength++] = current;
        current = parent[current];
    }
    
    printf("Camino de %d a %d: ", start, end);
    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) printf(" -> ");
    }
    printf(" (longitud: %d)\n", pathLength - 1);
    
    free(path);
}

void freeSearchResult(SearchResult* result) {
    if (result) {
        if (result->visitOrder) free(result->visitOrder);
        if (result->visited) free(result->visited);
        if (result->distances) free(result->distances);
        if (result->parent) free(result->parent);
        free(result);
    }
}