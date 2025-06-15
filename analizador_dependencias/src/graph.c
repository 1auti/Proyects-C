//
// Created by administrador on 6/9/25.
//

#include "../include/graph.h"
#include <stdio.h>
#include <stdlib.h>

Graph* create_graph(int vertices) {
    if (vertices <= 0 || vertices > MAX_VERTICES) {
        return NULL;
    }

    Graph* graph = malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->vertices = vertices;
    graph->edges = 0;

    // Allocar lista de adyacencia
    graph->adj_list = malloc(vertices * sizeof(int*));
    graph->adj_count = calloc(vertices, sizeof(int));

    if (!graph->adj_list || !graph->adj_count) {
        free(graph);
        return NULL;
    }

    for (int i = 0; i < vertices; i++) {
        graph->adj_list[i] = malloc(vertices * sizeof(int));
        if (!graph->adj_list[i]) {
            // Cleanup en caso de error
            for (int j = 0; j < i; j++) {
                free(graph->adj_list[j]);
            }
            free(graph->adj_list);
            free(graph->adj_count);
            free(graph);
            return NULL;
        }
    }

    return graph;
}

void destroy_graph(Graph* graph) {
    if (!graph) return;

    if (graph->adj_list) {
        for (int i = 0; i < graph->vertices; i++) {
            free(graph->adj_list[i]);
        }
        free(graph->adj_list);
    }

    free(graph->adj_count);
    free(graph);
}

void add_edge(Graph* graph, int src, int dest) {
    if (!graph || src < 0 || src >= graph->vertices ||
        dest < 0 || dest >= graph->vertices) {
        return;
    }

    // Verificar si la arista ya existe
    if (has_edge(graph, src, dest)) {
        return;
    }

    graph->adj_list[src][graph->adj_count[src]++] = dest;
    graph->edges++;
}

bool has_edge(Graph* graph, int src, int dest) {
    if (!graph || src < 0 || src >= graph->vertices ||
        dest < 0 || dest >= graph->vertices) {
        return false;
    }

    for (int i = 0; i < graph->adj_count[src]; i++) {
        if (graph->adj_list[src][i] == dest) {
            return true;
        }
    }
    return false;
}

void print_graph(Graph* graph, const char* vertex_names[]) {
    if (!graph) return;

    printf("\n=== GRAFO DE DEPENDENCIAS ===\n");
    for (int i = 0; i < graph->vertices; i++) {
        if (vertex_names && vertex_names[i]) {
            printf("%s -> ", vertex_names[i]);
        } else {
            printf("%d -> ", i);
        }

        for (int j = 0; j < graph->adj_count[i]; j++) {
            int dep = graph->adj_list[i][j];
            if (vertex_names && vertex_names[dep]) {
                printf("%s ", vertex_names[dep]);
            } else {
                printf("%d ", dep);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int get_vertex_count(Graph* graph) {
    return graph ? graph->vertices : 0;
}

int get_edge_count(Graph* graph) {
    return graph ? graph->edges : 0;
}