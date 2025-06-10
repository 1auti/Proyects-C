#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//ordenacion topologica con dfs
int main(void) {
    printf("Hello, World!\n");
    return 0;
}


void busqueda_topologica_dfs_util(Graph * graph, int v , bool * visited, int * stack, int * stack_top) {
    visited[v] = true;

    // Rercoremos todo los vertices de adyacencia
    for(int i = 0 ; i < graph->adj_count[v]; i++) {
        int u = graph->adj_count[v][i];
        if(!visited[u]) {
            busqueda_topologica_dfs_util(graph,u,visited,stack,stack_top);
        }
    }
     // agregar el vertice actual al stack
    stack[++(*stack_top)] = v;
}

void busqueda_topologica_dfs(Graph * graph) {
    int * stack = malloc(graph->vertices * sizeof(int));
    int stack_top = -1;
    bool * visited = calloc(graph->vertices * sizeof(bool));

    // Llamamos a la funcion auxiliar para todos los vertices
    for(int i = 0 ; i < graph->vertices; i++) {
        if(!visited[i]) {
            busqueda_topologica_dfs_util(graph,i,visited,stack,stack_top);
        }
    }
    printf("Ordenacion topologica (DFS)");
    while (stack_top >= 0 ) {
        printf("%d", stack[stack_top --]);
    }
    printf("\n");

    free(stack);
    free(visited);
}
