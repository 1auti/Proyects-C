#include "graph.h"

// Agregar arista usando matriz de adyacencia
void addEdgeMatrix(Graph* graph, int src, int dest, int weight) {
    if (!graph || src < 0 || src >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        printf("❌ Error: Vértices fuera de rango (%d -> %d)\n", src, dest);
        return;
    }
    
    // Agregar arista en la matriz
    graph->adjMatrix[src][dest] = weight;
    
    // Si el grafo no es dirigido, agregar en ambas direcciones
    if (!graph->isDirected) {
        graph->adjMatrix[dest][src] = weight;
    }
    
    printf("✅ Arista agregada en matriz: %d -> %d (peso: %d)\n", src, dest, weight);
}

// Remover arista de matriz de adyacencia
void removeEdgeMatrix(Graph* graph, int src, int dest) {
    if (!graph || src < 0 || src >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        printf("❌ Error: Vértices fuera de rango\n");
        return;
    }
    
    // Verificar si la arista existe
    if (graph->adjMatrix[src][dest] == 0) {
        printf("⚠️  Arista no existe: %d -> %d\n", src, dest);
        return;
    }
    
    // Remover arista
    graph->adjMatrix[src][dest] = 0;
    
    // Si el grafo no es dirigido, remover en ambas direcciones
    if (!graph->isDirected) {
        graph->adjMatrix[dest][src] = 0;
    }
    
    printf("✅ Arista removida de matriz: %d -> %d\n", src, dest);
}

// Imprimir grafo usando matriz de adyacencia
void printGraphMatrix(Graph* graph) {
    if (!graph) {
        printf("❌ Grafo nulo\n");
        return;
    }
    
    printf("\n📊 === REPRESENTACIÓN CON MATRIZ DE ADYACENCIA ===\n");
    printf("Tipo: %s\n", graph->isDirected ? "Dirigido" : "No dirigido");
    printf("Vértices: %d\n\n", graph->numVertices);
    
    // Imprimir header de columnas
    printf("     ");
    for (int i = 0; i < graph->numVertices; i++) {
        printf("%4d", i);
    }
    printf("\n");
    
    // Imprimir línea separadora
    printf("     ");
    for (int i = 0; i < graph->numVertices; i++) {
        printf("----");
    }
    printf("\n");
    
    // Imprimir filas con datos
    for (int i = 0; i < graph->numVertices; i++) {
        printf("%2d | ", i);
        for (int j = 0; j < graph->numVertices; j++) {
            if (graph->adjMatrix[i][j] == 0) {
                printf("   .");
            } else {
                printf("%4d", graph->adjMatrix[i][j]);
            }
        }
        printf("\n");
    }
    printf("================================================\n");
}

// Verificar si existe arista en matriz
bool existeAristaMatrix(Graph* graph, int src, int dest) {
    if (!graph || src < 0 || src >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        return false;
    }
    
    return graph->adjMatrix[src][dest] != 0;
}

// Obtener peso de arista en matriz
int obtenerPesoMatrix(Graph* graph, int src, int dest) {
    if (!graph || src < 0 || src >= graph->numVertices || 
        dest < 0 || dest >= graph->numVertices) {
        return 0;
    }
    
    return graph->adjMatrix[src][dest];
}

// Sincronizar matriz con lista de adyacencia
void sincronizarMatrizConLista(Graph* graph) {
    if (!graph) return;
    
    // Limpiar matriz
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->numVertices; j++) {
            graph->adjMatrix[i][j] = 0;
        }
    }
    
    // Llenar matriz basándose en listas de adyacencia
    for (int i = 0; i < graph->numVertices; i++) {
        AdjNode* current = graph->adj_list[i].raiz;
        while (current) {
            graph->adjMatrix[i][current->vertice] = current->peso;
            current = current->siguiente;
        }
    }
    
    printf("✅ Matriz sincronizada con listas de adyacencia\n");
}

// Sincronizar lista con matriz de adyacencia
void sincronizarListaConMatriz(Graph* graph) {
    if (!graph) return;
    
    // Limpiar listas de adyacencia
    for (int i = 0; i < graph->numVertices; i++) {
        AdjNode* current = graph->adj_list[i].raiz;
        while (current) {
            AdjNode* temp = current;
            current = current->siguiente;
            free(temp);
        }
        graph->adj_list[i].raiz = NULL;
    }
    
    // Llenar listas basándose en matriz
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->numVertices; j++) {
            if (graph->adjMatrix[i][j] != 0) {
                AdjNode* newNode = crearAdjNode(j, graph->adjMatrix[i][j]);
                if (newNode) {
                    newNode->siguiente = graph->adj_list[i].raiz;
                    graph->adj_list[i].raiz = newNode;
                }
            }
        }
    }
    
    printf("✅ Listas sincronizadas con matriz de adyacencia\n");
}

// Comparar representaciones (lista vs matriz)
bool compararRepresentaciones(Graph* graph) {
    if (!graph) return false;
    
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->numVertices; j++) {
            // Verificar si existe en matriz
            bool existeEnMatriz = (graph->adjMatrix[i][j] != 0);
            
            // Verificar si existe en lista
            bool existeEnLista = false;
            AdjNode* current = graph->adj_list[i].raiz;
            while (current) {
                if (current->vertice == j) {
                    existeEnLista = true;
                    // Verificar también si los pesos coinciden
                    if (current->peso != graph->adjMatrix[i][j]) {
                        printf("⚠️  Peso diferente en (%d,%d): Lista=%d, Matriz=%d\n", 
                               i, j, current->peso, graph->adjMatrix[i][j]);
                        return false;
                    }
                    break;
                }
                current = current->siguiente;
            }
            
            // Si no coinciden las representaciones
            if (existeEnMatriz != existeEnLista) {
                printf("⚠️  Inconsistencia en (%d,%d): Matriz=%s, Lista=%s\n", 
                       i, j, existeEnMatriz ? "SÍ" : "NO", existeEnLista ? "SÍ" : "NO");
                return false;
            }
        }
    }
    
    return true;
}

// Obtener densidad del grafo
double obtenerDensidad(Graph* graph) {
    if (!graph || graph->numVertices <= 1) return 0.0;
    
    int aristasActuales = 0;
    
    // Contar aristas en la matriz
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->numVertices; j++) {
            if (graph->adjMatrix[i][j] != 0) {
                aristasActuales++;
            }
        }
    }
    
    // Calcular máximo de aristas posibles
    int maxAristas;
    if (graph->isDirected) {
        maxAristas = graph->numVertices * (graph->numVertices - 1);
    } else {
        maxAristas = graph->numVertices * (graph->numVertices - 1) / 2;
        aristasActuales /= 2; // En grafo no dirigido, cada arista se cuenta dos veces
    }
    
    return (double)aristasActuales / maxAristas;
}

// Mostrar estadísticas de la matriz
void mostrarEstadisticasMatrix(Graph* graph) {
    if (!graph) {
        printf("❌ Grafo nulo\n");
        return;
    }
    
    printf("\n📊 === ESTADÍSTICAS DE LA MATRIZ ===\n");
    printf("🔢 Dimensión: %dx%d\n", graph->numVertices, graph->numVertices);
    printf("📏 Densidad: %.3f (%.1f%%)\n", obtenerDensidad(graph), obtenerDensidad(graph) * 100);
    
    // Verificar consistencia
    bool consistente = compararRepresentaciones(graph);
    printf("🔄 Consistencia Lista-Matriz: %s\n", consistente ? "✅ SÍ" : "❌ NO");
    
    printf("==================================\n");
}