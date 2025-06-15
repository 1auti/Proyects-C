//
// Created by administrador on 6/9/25.
//

#ifndef TARJAN_H
#define TARJAN_H

#include "graph.h"
#include "project.h"
#include <stdbool.h>

typedef struct {
    int* disc;              // Tiempo de descubrimiento
    int* low;               // Valor low más pequeño alcanzable
    bool* on_stack;         // Si está en el stack
    int* stack;             // Stack para SCC
    int stack_top;          // Tope del stack
    int time;               // Contador de tiempo
    int scc_count;          // Número de SCC encontrados
    int** scc_components;   // Matriz para almacenar componentes
    int* scc_sizes;         // Tamaños de cada SCC
} TarjanData;

// Funciones para análisis de componentes fuertemente conexos
TarjanData* create_tarjan_data(int vertices);
void destroy_tarjan_data(TarjanData* data);
void find_strongly_connected_components(Graph* graph, TarjanData* data);
void analyze_circular_dependencies(CodeProject* project);
void print_scc_results(const TarjanData* data, const CodeProject* project);
bool has_circular_dependencies(const TarjanData* data);

#endif //TARJAN_H
