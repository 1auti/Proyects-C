#include "priority_queue.h"

// Crear cola de prioridad
PriorityQueue* createPriorityQueue(int initialCapacity, bool isMinHeap) {
    if (initialCapacity <= 0) {
        initialCapacity = 16;
    }
    
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    
    pq->elements = (PQElement*)malloc(initialCapacity * sizeof(PQElement));
    if (!pq->elements) {
        free(pq);
        return NULL;
    }
    
    pq->size = 0;
    pq->capacity = initialCapacity;
    pq->isMinHeap = isMinHeap;
    
    return pq;
}

// Verificar si necesita intercambio según tipo de heap
bool shouldSwap(PriorityQueue* pq, int parent, int child) {
    if (pq->isMinHeap) {
        return pq->elements[parent].priority > pq->elements[child].priority;
    } else {
        return pq->elements[parent].priority < pq->elements[child].priority;
    }
}

// Redimensionar cuando se llena
void resizePriorityQueue(PriorityQueue* pq) {
    int newCapacity = pq->capacity * 2;
    PQElement* newElements = (PQElement*)realloc(pq->elements, 
                                                newCapacity * sizeof(PQElement));
    if (newElements) {
        pq->elements = newElements;
        pq->capacity = newCapacity;
    }
}

// Heapify hacia arriba
void heapifyUp(PriorityQueue* pq, int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        
        if (!shouldSwap(pq, parentIndex, index)) {
            break;
        }
        
        // Intercambiar
        PQElement temp = pq->elements[index];
        pq->elements[index] = pq->elements[parentIndex];
        pq->elements[parentIndex] = temp;
        
        index = parentIndex;
    }
}

// Heapify hacia abajo
void heapifyDown(PriorityQueue* pq, int index) {
    while (true) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int targetIndex = index;
        
        // Encontrar el índice objetivo según el tipo de heap
        if (leftChild < pq->size && shouldSwap(pq, targetIndex, leftChild)) {
            targetIndex = leftChild;
        }
        
        if (rightChild < pq->size && shouldSwap(pq, targetIndex, rightChild)) {
            targetIndex = rightChild;
        }
        
        if (targetIndex == index) {
            break; // Ya está en la posición correcta
        }
        
        // Intercambiar
        PQElement temp = pq->elements[index];
        pq->elements[index] = pq->elements[targetIndex];
        pq->elements[targetIndex] = temp;
        
        index = targetIndex;
    }
}

// Insertar elemento
bool pqPush(PriorityQueue* pq, void* data, int priority) {
    if (!pq) return false;
    
    // Redimensionar si es necesario
    if (pq->size >= pq->capacity) {
        resizePriorityQueue(pq);
        if (pq->size >= pq->capacity) {
            return false; // Falló el resize
        }
    }
    
    // Insertar al final
    pq->elements[pq->size].data = data;
    pq->elements[pq->size].priority = priority;
    
    // Heapify hacia arriba
    heapifyUp(pq, pq->size);
    pq->size++;
    
    return true;
}

// Extraer elemento de mayor/menor prioridad
void* pqPop(PriorityQueue* pq) {
    if (!pq || pq->size == 0) return NULL;
    
    void* data = pq->elements[0].data;
    
    // Mover último elemento al inicio
    pq->elements[0] = pq->elements[pq->size - 1];
    pq->size--;
    
    // Heapify hacia abajo si no está vacío
    if (pq->size > 0) {
        heapifyDown(pq, 0);
    }
    
    return data;
}

// Ver elemento de mayor/menor prioridad sin extraer
void* pqPeek(PriorityQueue* pq) {
    if (!pq || pq->size == 0) return NULL;
    return pq->elements[0].data;
}

// Verificar si está vacía
bool pqIsEmpty(PriorityQueue* pq) {
    return !pq || pq->size == 0;
}

// Obtener tamaño
int pqSize(PriorityQueue* pq) {
    return pq ? pq->size : 0;
}

// Limpiar todos los elementos
void pqClear(PriorityQueue* pq) {
    if (pq) {
        pq->size = 0;
    }
}

// Verificar si contiene un elemento
bool pqContains(PriorityQueue* pq, void* data) {
    if (!pq) return false;
    
    for (int i = 0; i < pq->size; i++) {
        if (pq->elements[i].data == data) {
            return true;
        }
    }
    return false;
}

// Actualizar prioridad de un elemento
bool pqUpdatePriority(PriorityQueue* pq, void* data, int newPriority) {
    if (!pq) return false;
    
    // Encontrar el elemento
    int index = -1;
    for (int i = 0; i < pq->size; i++) {
        if (pq->elements[i].data == data) {
            index = i;
            break;
        }
    }
    
    if (index == -1) return false; // No encontrado
    
    int oldPriority = pq->elements[index].priority;
    pq->elements[index].priority = newPriority;
    
    // Decidir dirección del heapify
    if ((pq->isMinHeap && newPriority < oldPriority) || 
        (!pq->isMinHeap && newPriority > oldPriority)) {
        heapifyUp(pq, index);
    } else {
        heapifyDown(pq, index);
    }
    
    return true;
}

// Imprimir cola de prioridad
void printPriorityQueue(PriorityQueue* pq) {
    if (!pq) {
        printf("Priority Queue es NULL\n");
        return;
    }
    
    printf("\n=== PRIORITY QUEUE ===\n");
    printf("Tipo: %s\n", pq->isMinHeap ? "Min-Heap" : "Max-Heap");
    printf("Tamaño: %d/%d\n", pq->size, pq->capacity);
    
    if (pq->size == 0) {
        printf("Cola vacía\n");
    } else {
        printf("Elementos (prioridad):\n");
        for (int i = 0; i < pq->size; i++) {
            printf("  [%d]: prioridad %d\n", i, pq->elements[i].priority);
        }
        
        if (pq->size > 0) {
            printf("Siguiente elemento: prioridad %d\n", pq->elements[0].priority);
        }
    }
    printf("======================\n");
}

// Destruir cola de prioridad
void destroyPriorityQueue(PriorityQueue* pq) {
    if (!pq) return;
    
    if (pq->elements) {
        free(pq->elements);
    }
    free(pq);
}