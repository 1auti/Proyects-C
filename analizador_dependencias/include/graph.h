//
// Created by administrador on 6/9/25.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

#define MAX_VERTICES 100

typedef struct {
    int** adj_list;     // Lista de adyacencia
    int* adj_count;     // Contador de adyacentes por vértice
    int vertices;       // Número de vértices
    int edges;          // Número de aristas
} Graph;

// Funciones públicas del grafo
Graph* create_graph(int vertices);
void destroy_graph(Graph* graph);
void add_edge(Graph* graph, int src, int dest);
void print_graph(Graph* graph, const char* vertex_names[]);
bool has_edge(Graph* graph, int src, int dest);
int get_vertex_count(Graph* graph);
int get_edge_count(Graph* graph);

#endif //GRAPH_H
