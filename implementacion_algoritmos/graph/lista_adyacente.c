#include "graph.h"

// Agregar arista usando lista de adyacencia
bool addEdgeList(Graph* graph, int src, int dest, int weight) {
    if (!graph || src < 0 || dest < 0 || src >= graph->numVertices || dest >= graph->numVertices) {
        return false;
    }

    // Crear nuevo nodo para la lista de adyacencia
    AdjNode* newNode = (AdjNode*)malloc(sizeof(AdjNode));
    if (!newNode) {
        return false;
    }

    // CORREGIDO: usar los nombres correctos de tu estructura
    newNode->vertice = dest;        // Tu estructura usa 'vertice', no 'dest'
    newNode->peso = weight;         // Tu estructura usa 'peso', no 'weight'
    newNode->siguiente = graph->adj_list[src].raiz;  // Tu estructura usa 'siguiente' y 'adj_list[].raiz'
    graph->adj_list[src].raiz = newNode;             // No 'graph->array[].head'

    // Si el grafo es no dirigido, agregar la arista en ambas direcciones
    if (!graph->isDirected) {  // Tu estructura usa 'isDirected', no 'directed'
        AdjNode* newNode2 = (AdjNode*)malloc(sizeof(AdjNode));  // CORREGIDO: AdjNode, no AdjListNode
        if (!newNode2) {
            // Limpiar el primer nodo si falla la asignación del segundo
            return false;
        }
        newNode2->vertice = src;    // CORREGIDO: usar 'vertice'
        newNode2->peso = weight;    // CORREGIDO: usar 'peso'
        newNode2->siguiente = graph->adj_list[dest].raiz;  // CORREGIDO: usar la estructura correcta
        graph->adj_list[dest].raiz = newNode2;
    }

    return true;
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