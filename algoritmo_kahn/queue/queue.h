//
// Created by administrador on 6/9/25.
//

#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include "../graph/graph.h"

typedef struct Queue {
    int * data; // arreglo donde se almacena la plata
    int front, rear, size; // front es el primer indice donde se alamcaena la cola,
                           // rear es el ultimo indice donde se almacena la cola
                           // size seria el tamanio de la cola
}Queue;

// Funciion para craer una cola
Queue * create_queue(int capacity); // Le pedimos la capacidad para reserver la memoria para la cola
void enqueue(Queue * queue, int iteam); // insertar un elemento de la cola
int dequeue(Queue * queue); // eliminar un elemento de la cola
bool topological_sort_kahn(Graph * graph); // es una ordenacion topologica con el alg kahn








#endif //QUEUE_H
