//
// Created by administrador on 6/9/25.
//

#include "../include/tarjan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TarjanData* create_tarjan_data(int vertices) {
    if (vertices <= 0) return NULL;

    TarjanData* data = malloc(sizeof(TarjanData));
    if (!data) return NULL;

    data->disc = malloc(vertices * sizeof(int));
    data->low = malloc(vertices * sizeof(int));
    data->on_stack = calloc(vertices, sizeof(bool));
    data->stack = malloc(vertices * sizeof(int));
    data->scc_components = malloc(vertices * sizeof(int*));
    data->scc_sizes = calloc(vertices, sizeof(int));

    if (!data->disc || !data->low || !data->on_stack ||
        !data->stack || !data->scc_components || !data->scc_sizes) {
        destroy_tarjan_data(data);
        return NULL;
    }

    for (int i = 0; i < vertices; i++) {
        data->scc_components[i] = malloc(vertices * sizeof(int));
        if (!data->scc_components[i]) {
            destroy_tarjan_data(data);
            return NULL;
        }
        data->disc[i] = -1;
        data->low[i] = -1;
    }

    data->stack_top = -1;
    data->time = 0;
    data->scc_count = 0;

    return data;
}

void destroy_tarjan_data(TarjanData* data) {
    if (!data) return;

    free(data->disc);
    free(data->low);
    free(data->on_stack);
    free(data->stack);
    free(data->scc_sizes);

    if (data->scc_components) {
        for (int i = 0; i < data->scc_count; i++) {
            free(data->scc_components[i]);
        }
        free(data->scc_components);
    }

    free(data);
}

static void tarjan_scc_util(Graph* graph, int u, TarjanData* data) {
    data->disc[u] = data->low[u] = ++data->time;
    data->stack[++data->stack_top] = u;
    data->on_stack[u] = true;

    // Recorrer vértices adyacentes
    for (int i = 0; i < graph->adj_count[u]; i++) {
        int v = graph->adj_list[u][i];

        if (data->disc[v] == -1) {
            tarjan_scc_util(graph, v, data);
            data->low[u] = (data->low[u] < data->low[v]) ? data->low[u] : data->low[v];
        }
        else if (data->on_stack[v]) {
            data->low[u] = (data->low[u] < data->disc[v]) ? data->low[u] : data->disc[v];
        }
    }

    // Si u es raíz de SCC
    if (data->low[u] == data->disc[u]) {
        int scc_size = 0;
        int w;

        do {
            w = data->stack[data->stack_top--];
            data->on_stack[w] = false;
            data->scc_components[data->scc_count][scc_size++] = w;
        } while (w != u);

        data->scc_sizes[data->scc_count] = scc_size;
        data->scc_count++;
    }
}

void find_strongly_connected_components(Graph* graph, TarjanData* data) {
    if (!graph || !data) return;

    // Resetear datos
    for (int i = 0; i < graph->vertices; i++) {
        data->disc[i] = -1;
        data->low[i] = -1;
        data->on_stack[i] = false;
    }
    data->stack_top = -1;
    data->time = 0;
    data->scc_count = 0;

    // Ejecutar Tarjan para todos los vértices
    for (int i = 0; i < graph->vertices; i++) {
        if (data->disc[i] == -1) {
            tarjan_scc_util(graph, i, data);
        }
    }
}

void analyze_circular_dependencies(CodeProject* project) {
    if (!project || !project->dependency_graph) return;

    TarjanData* data = create_tarjan_data(project->module_count);
    if (!data) return;

    printf("\n🔍 ANALIZANDO DEPENDENCIAS CIRCULARES...\n");

    find_strongly_connected_components(project->dependency_graph, data);
    print_scc_results(data, project);

    destroy_tarjan_data(data);
}

void print_scc_results(const TarjanData* data, const CodeProject* project) {
    if (!data || !project) return;

    int circular_count = 0;

    for (int i = 0; i < data->scc_count; i++) {
        if (data->scc_sizes[i] > 1) {
            circular_count++;
            printf("🔴 DEPENDENCIA CIRCULAR #%d:\n", circular_count);
            printf("   Módulos involucrados: ");

            for (int j = 0; j < data->scc_sizes[i]; j++) {
                int module_id = data->scc_components[i][j];
                printf("%s ", project->modules[module_id].name);
            }
            printf("\n   ⚠️  Esto puede causar problemas de compilación!\n\n");
        }
    }

    if (circular_count == 0) {
        printf("✅ No se encontraron dependencias circulares!\n");
    } else {
        printf("📊 Total de dependencias circulares: %d\n", circular_count);
    }
}

bool has_circular_dependencies(const TarjanData* data) {
    if (!data) return false;

    for (int i = 0; i < data->scc_count; i++) {
        if (data->scc_sizes[i] > 1) {
            return true;
        }
    }
    return false;
}