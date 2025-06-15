//
// Created by administrador on 6/9/25.
//

#ifndef GRAPH_H
#define GRAPH_H
#define MAX_VERTICES 100
#define NIL -1

typedef struct {
    int** adj_list;     // Lista de adyacencia
    int* adj_count;     // Contador de adyacentes por vértice
    int vertices;       // Número de vértices
    int edges;          // Número de aristas
} Graph;

Graph* create_graph(int vertices);
void add_edge(Graph* graph, int src, int dest);

#endif //GRAPH_H
