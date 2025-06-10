//
// Created by administrador on 6/9/25.
//

#ifndef TOPOLOGICAL_H
#define TOPOLOGICAL_H

#include "graph.h"
#include "project.h"
#include <stdbool.h>

typedef struct {
    int* order;             // Orden topológico resultante
    int* in_degree;         // Grado de entrada de cada vértice
    int* queue;             // Cola para procesamiento
    int order_count;        // Número de elementos en el orden
    bool is_valid;          // Si el orden es válido (sin ciclos)
} TopologicalResult;

// Funciones para ordenación topológica
TopologicalResult* create_topological_result(int vertices);
void destroy_topological_result(TopologicalResult* result);
bool topological_sort_kahn(Graph* graph, TopologicalResult* result);
bool topological_sort_dfs(Graph* graph, TopologicalResult* result);
void print_compilation_order(const TopologicalResult* result, const CodeProject* project);
bool generate_build_script(const TopologicalResult* result, const CodeProject* project, const char* filename);

#endif //TOPOLOGICAL_H
