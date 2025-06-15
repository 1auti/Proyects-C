#include "../include/topological.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TopologicalResult* create_topological_result(int vertices) {
    if (vertices <= 0) return NULL;
    
    TopologicalResult* result = malloc(sizeof(TopologicalResult));
    if (!result) return NULL;
    
    result->order = malloc(vertices * sizeof(int));
    result->in_degree = malloc(vertices * sizeof(int));
    result->queue = malloc(vertices * sizeof(int));
    
    if (!result->order || !result->in_degree || !result->queue) {
        destroy_topological_result(result);
        return NULL;
    }
    
    result->order_count = 0;
    result->is_valid = false;
    
    return result;
}

void destroy_topological_result(TopologicalResult* result) {
    if (!result) return;
    
    free(result->order);
    free(result->in_degree);
    free(result->queue);
    free(result);
}

bool topological_sort_kahn(Graph* graph, TopologicalResult* result) {
    if (!graph || !result) return false;
    
    int vertices = graph->vertices;
    
    // Inicializar grados de entrada
    for (int i = 0; i < vertices; i++) {
        result->in_degree[i] = 0;
    }
    
    // Calcular grados de entrada
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < graph->adj_count[i]; j++) {
            result->in_degree[graph->adj_list[i][j]]++;
        }
    }
    
    // Inicializar cola con vértices de grado 0
    int front = 0, rear = -1;
    for (int i = 0; i < vertices; i++) {
        if (result->in_degree[i] == 0) {
            result->queue[++rear] = i;
        }
    }
    
    result->order_count = 0;
    
    // Procesar vértices
    while (front <= rear) {
        int u = result->queue[front++];
        result->order[result->order_count++] = u;
        
        // Reducir grado de entrada de vértices adyacentes
        for (int i = 0; i < graph->adj_count[u]; i++) {
            int v = graph->adj_list[u][i];
            result->in_degree[v]--;
            
            if (result->in_degree[v] == 0) {
                result->queue[++rear] = v;
            }
        }
    }
    
    // Verificar si es posible (sin ciclos)
    result->is_valid = (result->order_count == vertices);
    return result->is_valid;
}

static void topological_dfs_util(Graph* graph, int v, bool* visited, int* stack, int* stack_top) {
    visited[v] = true;
    
    for (int i = 0; i < graph->adj_count[v]; i++) {
        int u = graph->adj_list[v][i];
        if (!visited[u]) {
            topological_dfs_util(graph, u, visited, stack, stack_top);
        }
    }
    
    stack[++(*stack_top)] = v;
}

bool topological_sort_dfs(Graph* graph, TopologicalResult* result) {
    if (!graph || !result) return false;
    
    int vertices = graph->vertices;
    bool* visited = calloc(vertices, sizeof(bool));
    int* stack = malloc(vertices * sizeof(int));
    int stack_top = -1;
    
    if (!visited || !stack) {
        free(visited);
        free(stack);
        return false;
    }
    
    // DFS para todos los vértices no visitados
    for (int i = 0; i < vertices; i++) {
        if (!visited[i]) {
            topological_dfs_util(graph, i, visited, stack, &stack_top);
        }
    }
    
    // Copiar resultado desde el stack (en orden reverso)
    result->order_count = 0;
    while (stack_top >= 0) {
        result->order[result->order_count++] = stack[stack_top--];
    }
    
    result->is_valid = true; // DFS siempre produce un orden válido
    
    free(visited);
    free(stack);
    return true;
}

void print_compilation_order(const TopologicalResult* result, const CodeProject* project) {
    if (!result || !project) return;
    
    if (!result->is_valid) {
        printf("❌ No es posible generar orden de compilación debido a dependencias circulares.\n");
        return;
    }
    
    printf("\n📋 ORDEN DE COMPILACIÓN SUGERIDO:\n");
    printf("=================================\n");
    
    for (int i = 0; i < result->order_count; i++) {
        int module_id = result->order[i];
        printf("%d. %s\n", i + 1, project->modules[module_id].name);
    }
    printf("\n");
}

bool generate_build_script(const TopologicalResult* result, const CodeProject* project, const char* filename) {
    if (!result || !project || !filename || !result->is_valid) {
        return false;
    }
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "#!/bin/bash\n");
    fprintf(file, "# Script de compilación generado automáticamente\n");
    fprintf(file, "# Proyecto: %s\n\n", project->project_name);
    
    fprintf(file, "echo \"Compilando proyecto: %s\"\n", project->project_name);
    fprintf(file, "echo \"Orden de compilación determinado por análisis de dependencias\"\n\n");
    
    for (int i = 0; i < result->order_count; i++) {
        int module_id = result->order[i];
        const char* module_name = project->modules[module_id].name;
        
        fprintf(file, "echo \"Compilando módulo %d/%d: %s\"\n", 
                i + 1, result->order_count, module_name);
        fprintf(file, "gcc -c src/%s.c -o obj/%s.o\n", module_name, module_name);
        fprintf(file, "if [ $? -ne 0 ]; then\n");
        fprintf(file, "    echo \"Error compilando %s\"\n", module_name);
        fprintf(file, "    exit 1\n");
        fprintf(file, "fi\n\n");
    }
    
    fprintf(file, "echo \"Enlazando ejecutable final...\"\n");
    fprintf(file, "gcc obj/*.o -o bin/%s\n", project->project_name);
    fprintf(file, "echo \"Compilación completada exitosamente!\"\n");
    
    fclose(file);
    printf("Script de compilación generado: %s\n", filename);
    return true;
}