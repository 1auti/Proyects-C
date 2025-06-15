//
// Created by administrador on 6/9/25.
//

#include "../queue/queue.h"

#include <stdio.h>
#include <stdlib.h>

Queue * create_queue(int capacity) {
    Queue * queue = malloc(sizeof(Queue));
    queue->data = malloc(capacity * sizeof(int));
    queue->front = queue->rear = -1;
    queue->size = 0;
    return queue;
}

int dequeue(Queue* queue) {
    if (queue->size == 0) return -1;

    int item = queue->data[queue->front];
    if (queue->front == queue->rear) {
        queue->front = queue->rear = -1;
    } else {
        queue->front++;
    }
    queue->size--;
    return item;
}

bool topological_sort_kahn(Graph* graph) {
    int* in_degree = calloc(graph->vertices, sizeof(int));
    Queue* queue = create_queue(graph->vertices);
    int* result = malloc(graph->vertices * sizeof(int));
    int result_index = 0;

    // Calcular grado de entrada para todos los vértices
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = 0; j < graph->adj_count[i]; j++) {
            in_degree[graph->adj_list[i][j]]++;
        }
    }

    // Agregar todos los vértices con grado de entrada 0 a la cola
    for (int i = 0; i < graph->vertices; i++) {
        if (in_degree[i] == 0) {
            enqueue(queue, i);
        }
    }

    // Procesar vértices uno por uno
    while (queue->size > 0) {
        int u = dequeue(queue);
        result[result_index++] = u;

        // Para cada vértice adyacente a u
        for (int i = 0; i < graph->adj_count[u]; i++) {
            int v = graph->adj_list[u][i];
            in_degree[v]--;

            // Si el grado de entrada se vuelve 0, agregarlo a la cola
            if (in_degree[v] == 0) {
                enqueue(queue, v);
            }
        }
    }

    // Verificar si hay un ciclo
    if (result_index != graph->vertices) {
        printf("El grafo contiene un ciclo. No es posible la ordenación topológica.\n");
        free(in_degree);
        free(result);
        return false;
    }

    printf("Ordenación topológica: ");
    for (int i = 0; i < result_index; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    free(in_degree);
    free(result);
    return true;
}
