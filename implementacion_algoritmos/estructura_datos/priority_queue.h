#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Elemento de la cola de prioridad
typedef struct {
    void* data;
    int priority;
} PQElement;

// Cola de prioridad (min-heap por defecto)
typedef struct {
    PQElement* elements;
    int size;
    int capacity;
    bool isMinHeap;
} PriorityQueue;

// Prototipos de funciones principales
PriorityQueue* createPriorityQueue(int initialCapacity, bool isMinHeap);
void destroyPriorityQueue(PriorityQueue* pq);

// Operaciones básicas
bool pqPush(PriorityQueue* pq, void* data, int priority);
void* pqPop(PriorityQueue* pq);
void* pqPeek(PriorityQueue* pq);
bool pqIsEmpty(PriorityQueue* pq);
int pqSize(PriorityQueue* pq);

// Utilidades
void pqClear(PriorityQueue* pq);
void printPriorityQueue(PriorityQueue* pq);
bool pqContains(PriorityQueue* pq, void* data);
bool pqUpdatePriority(PriorityQueue* pq, void* data, int newPriority);

// Funciones auxiliares del heap
void heapifyUp(PriorityQueue* pq, int index);
void heapifyDown(PriorityQueue* pq, int index);
void resizePriorityQueue(PriorityQueue* pq);
bool shouldSwap(PriorityQueue* pq, int parent, int child);

#endif