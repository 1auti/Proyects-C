//
// Created by administrador on 6/9/25.
//

#include "../graph/graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

Graph* create_graph(int vertices) {
    Graph* graph = malloc(sizeof(Graph));
    graph->vertices = vertices;
    graph->edges = 0;

    // Inicializar lista de adyacencia
    graph->adj_list = malloc(vertices * sizeof(int*));
    graph->adj_count = calloc(vertices, sizeof(int));

    for (int i = 0; i < vertices; i++) {
        graph->adj_list[i] = malloc(vertices * sizeof(int));
    }

    return graph;
}

// Agregar arista dirigida
void add_edge(Graph* graph, int src, int dest) {
    graph->adj_list[src][graph->adj_count[src]++] = dest;
    graph->edges++;
}