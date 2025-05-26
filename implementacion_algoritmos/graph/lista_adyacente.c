#include "graph.h"

// Agregar arista usando lista de adyacencia
void addEdgeList(Graph* graph, int raiz, int dest, int peso) {
    if (!graph || raiz < 0 || raiz >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        printf("❌ Error: Vértices fuera de rango (%d -> %d)\n", raiz, dest);
        return;
    }
    
    // Crear nuevo nodo y agregarlo al inicio de la lista
    AdjNode* newNode = crearAdjNode(dest, peso);
    if (!newNode) {
        printf("❌ Error: No se pudo crear nodo\n");
        return;
    }
    
    newNode->siguiente = graph->adj_list[raiz].raiz;
    graph->adj_list[raiz].raiz = newNode;
    
    // Si el grafo no es dirigido, agregar arista en dirección opuesta
    if (!graph->isDirected) {
        AdjNode* newNode2 = crearAdjNode(raiz, peso);
        if (newNode2) {
            newNode2->siguiente = graph->adj_list[dest].raiz;
            graph->adj_list[dest].raiz = newNode2;
        }
    }
    
    printf("✅ Arista agregada: %d -> %d (peso: %d)\n", raiz, dest, peso);
}

// Remover arista de lista de adyacencia
void removeEdgeList(Graph* graph, int raiz, int dest) {
    if (!graph || raiz < 0 || raiz >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        printf("❌ Error: Vértices fuera de rango\n");
        return;
    }
    
    // Remover de la lista del vértice raiz
    AdjNode* current = graph->adj_list[raiz].raiz;
    AdjNode* prev = NULL;
    
    while (current && current->vertice != dest) {
        prev = current;
        current = current->siguiente;
    }
    
    if (current) {
        if (prev) {
            prev->siguiente = current->siguiente;
        } else {
            graph->adj_list[raiz].raiz = current->siguiente;
        }
        free(current);
        printf("✅ Arista removida: %d -> %d\n", raiz, dest);
    } else {
        printf("⚠️  Arista no encontrada: %d -> %d\n", raiz, dest);
        return;
    }
    
    // Si el grafo no es dirigido, remover también en dirección opuesta
    if (!graph->isDirected) {
        current = graph->adj_list[dest].raiz;
        prev = NULL;
        
        while (current && current->vertice != raiz) {
            prev = current;
            current = current->siguiente;
        }
        
        if (current) {
            if (prev) {
                prev->siguiente = current->siguiente;
            } else {
                graph->adj_list[dest].raiz = current->siguiente;
            }
            free(current);
        }
    }
}

// Imprimir grafo usando listas de adyacencia
void printEdgeList(Graph* graph) {
    if (!graph) {
        printf("❌ Grafo nulo\n");
        return;
    }
    
    printf("\n📋 === REPRESENTACIÓN CON LISTAS DE ADYACENCIA ===\n");
    printf("Tipo: %s\n", graph->isDirected ? "Dirigido" : "No dirigido");
    printf("Vértices: %d\n\n", graph->numVertices);
    
    for (int i = 0; i < graph->numVertices; i++) {
        printf("Vértice %d: ", i);
        
        AdjNode* current = graph->adj_list[i].raiz;
        if (!current) {
            printf("(sin conexiones)");
        }
        
        while (current) {
            printf("-> %d(peso:%d) ", current->vertice, current->peso);
            current = current->siguiente;
        }
        printf("\n");
    }
    printf("===============================================\n");
}

// Verificar si existe una arista entre dos vértices
bool existeArista(Graph* graph, int raiz, int dest) {
    if (!graph || raiz < 0 || raiz >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        return false;
    }
    
    AdjNode* current = graph->adj_list[raiz].raiz;
    while (current) {
        if (current->vertice == dest) {
            return true;
        }
        current = current->siguiente;
    }
    
    return false;
}

// Obtener peso de una arista
int obtenerPesoArista(Graph* graph, int raiz, int dest) {
    if (!graph || raiz < 0 || raiz >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        return -1;
    }
    
    AdjNode* current = graph->adj_list[raiz].raiz;
    while (current) {
        if (current->vertice == dest) {
            return current->peso;
        }
        current = current->siguiente;
    }
    
    return -1; // Arista no encontrada
}

// Obtener grado de un vértice
int obtenerGrado(Graph* graph, int vertice) {
    if (!graph || vertice < 0 || vertice >= graph->numVertices) {
        return -1;
    }
    
    int grado = 0;
    AdjNode* current = graph->adj_list[vertice].raiz;
    
    while (current) {
        grado++;
        current = current->siguiente;
    }
    
    return grado;
}

// Obtener todos los vértices adyacentes a uno dado
int* obtenerAdyacentes(Graph* graph, int vertice, int* numAdyacentes) {
    *numAdyacentes = 0;
    
    if (!graph || vertice < 0 || vertice >= graph->numVertices) {
        return NULL;
    }
    
    // Contar adyacentes
    int grado = obtenerGrado(graph, vertice);
    if (grado == 0) return NULL;
    
    // Crear array para almacenar adyacentes
    int* adyacentes = (int*)malloc(grado * sizeof(int));
    int index = 0;
    
    AdjNode* current = graph->adj_list[vertice].raiz;
    while (current) {
        adyacentes[index++] = current->vertice;
        current = current->siguiente;
    }
    
    *numAdyacentes = grado;
    return adyacentes;
}

// Mostrar información detallada de un vértice
void mostrarInfoVertice(Graph* graph, int vertice) {
    if (!graph || vertice < 0 || vertice >= graph->numVertices) {
        printf("❌ Vértice inválido\n");
        return;
    }
    
    printf("\n🔍 === INFORMACIÓN DEL VÉRTICE %d ===\n", vertice);
    printf("Grado: %d\n", obtenerGrado(graph, vertice));
    
    printf("Adyacentes: ");
    AdjNode* current = graph->adj_list[vertice].raiz;
    if (!current) {
        printf("(ninguno)");
    }
    
    while (current) {
        printf("%d ", current->vertice);
        current = current->siguiente;
    }
    printf("\n================================\n");
}