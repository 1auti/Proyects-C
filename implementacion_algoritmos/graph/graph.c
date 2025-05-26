#include "graph.h"

// Crear un nuevo grafo
Graph* crearGraph(int vertices, bool isDirected) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;
    
    graph->numVertices = vertices;
    graph->isDirected = isDirected;
    
    // Inicializar lista de adyacencia
    graph->adj_list = (AdjList*)malloc(vertices * sizeof(AdjList));
    for (int i = 0; i < vertices; i++) {
        graph->adj_list[i].raiz = NULL;
    }
    
    // Inicializar matriz de adyacencia
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            graph->adjMatrix[i][j] = 0;
        }
    }
    
    printf("✅ Grafo creado: %d vértices, %s\n", 
           vertices, isDirected ? "dirigido" : "no dirigido");
    
    return graph;
}

// Destruir grafo y liberar memoria
void destruirGraph(Graph* graph) {
    if (!graph) return;
    
    // Liberar listas de adyacencia
    for (int i = 0; i < graph->numVertices; i++) {
        AdjNode* current = graph->adj_list[i].raiz;
        while (current) {
            AdjNode* temp = current;
            current = current->siguiente;
            free(temp);
        }
    }
    
    free(graph->adj_list);
    free(graph);
    printf("🗑️  Grafo destruido\n");
}

// Crear un nuevo nodo de adyacencia
AdjNode* crearAdjNode(int vertice, int peso) {
    AdjNode* newNode = (AdjNode*)malloc(sizeof(AdjNode));
    if (!newNode) return NULL;
    
    newNode->vertice = vertice;
    newNode->peso = peso;
    newNode->siguiente = NULL;
    return newNode;
}

// Función auxiliar DFS recursiva
void DFSUtil(Graph* graph, int vertice, bool visitado[]) {
    visitado[vertice] = true;
    printf("%d ", vertice);
    
    // Recorrer todos los vértices adyacentes usando lista de adyacencia
    AdjNode* current = graph->adj_list[vertice].raiz;
    while (current) {
        if (!visitado[current->vertice]) {
            DFSUtil(graph, current->vertice, visitado);
        }
        current = current->siguiente;
    }
}

// Búsqueda en profundidad (DFS)
void DFS(Graph* graph, int initVertice) {
    if (!graph || initVertice < 0 || initVertice >= graph->numVertices) {
        printf("❌ Error: Vértice inválido para DFS\n");
        return;
    }
    
    bool* visitado = (bool*)calloc(graph->numVertices, sizeof(bool));
    
    printf("🔍 DFS desde vértice %d: ", initVertice);
    DFSUtil(graph, initVertice, visitado);
    printf("\n");
    
    free(visitado);
}

// Búsqueda en anchura (BFS)
void BFS(Graph* graph, int initVertice) {
    if (!graph || initVertice < 0 || initVertice >= graph->numVertices) {
        printf("❌ Error: Vértice inválido para BFS\n");
        return;
    }
    
    bool* visitado = (bool*)calloc(graph->numVertices, sizeof(bool));
    int* cola = (int*)malloc(graph->numVertices * sizeof(int));
    int frente = 0, final = 0;
    
    printf("🔍 BFS desde vértice %d: ", initVertice);
    
    visitado[initVertice] = true;
    cola[final++] = initVertice;
    
    while (frente < final) {
        int verticeActual = cola[frente++];
        printf("%d ", verticeActual);
        
        // Recorrer vértices adyacentes usando lista de adyacencia
        AdjNode* current = graph->adj_list[verticeActual].raiz;
        while (current) {
            if (!visitado[current->vertice]) {
                visitado[current->vertice] = true;
                cola[final++] = current->vertice;
            }
            current = current->siguiente;
        }
    }
    
    printf("\n");
    free(visitado);
    free(cola);
}

// Función para verificar si el grafo está conectado
bool isGraphConnected(Graph* graph) {
    if (!graph || graph->numVertices <= 1) return true;
    
    bool* visitado = (bool*)calloc(graph->numVertices, sizeof(bool));
    
    // Realizar DFS desde el vértice 0
    DFSUtil(graph, 0, visitado);
    
    // Verificar si todos los vértices fueron visitados
    bool connected = true;
    for (int i = 0; i < graph->numVertices; i++) {
        if (!visitado[i]) {
            connected = false;
            break;
        }
    }
    
    free(visitado);
    return connected;
}

// Contar aristas en el grafo
int contarAristas(Graph* graph) {
    if (!graph) return 0;
    
    int count = 0;
    for (int i = 0; i < graph->numVertices; i++) {
        AdjNode* current = graph->adj_list[i].raiz;
        while (current) {
            count++;
            current = current->siguiente;
        }
    }
    
    // Si es no dirigido, dividir por 2 porque cada arista se cuenta dos veces
    return graph->isDirected ? count : count / 2;
}

// Mostrar estadísticas del grafo
void mostrarEstadisticas(Graph* graph) {
    if (!graph) {
        printf("❌ Grafo nulo\n");
        return;
    }
    
    printf("\n📊 === ESTADÍSTICAS DEL GRAFO ===\n");
    printf("🔢 Vértices: %d\n", graph->numVertices);
    printf("🔗 Aristas: %d\n", contarAristas(graph));
    printf("📐 Tipo: %s\n", graph->isDirected ? "Dirigido" : "No dirigido");
    printf("🌐 Conectado: %s\n", isGraphConnected(graph) ? "SÍ" : "NO");
    printf("===============================\n");
}