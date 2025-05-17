/*//
// Created by Lautaro on 12/05/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include "nodo.h"

// Crear un nuevo nodo con el valor dado
// ptrNodoArbol crearNodo(int valor) {
//     ptrNodoArbol nuevoNodo = (ptrNodoArbol)malloc(sizeof(NodoArbol));
//     if (nuevoNodo == NULL) {
//         printf("Error: No se pudo asignar memoria para el nuevo nodo\n");
//         exit(1);
//     }
//     nuevoNodo->dato = valor;
//     nuevoNodo->izquierdo = NULL;
//     nuevoNodo->derecho = NULL;
//     return nuevoNodo;
// }

// Insertar un valor en el árbol (BST)
ptrNodoArbol insertarNodo(ptrNodoArbol raiz, int valor) {
    // Si el árbol está vacío, crear un nuevo nodo como raíz
    if (raiz == NULL) {
        return crearNodo(valor);
    }

    // Si el valor es menor, insertar en subárbol izquierdo
    if (valor < raiz->dato) {
        raiz->izquierdo = insertarNodo(raiz->izquierdo, valor);
    }
    // Si el valor es mayor, insertar en subárbol derecho
    else if (valor > raiz->dato) {
        raiz->derecho = insertarNodo(raiz->derecho, valor);
    }
    // Si el valor ya existe, no hacer nada (evitar duplicados)

    return raiz;
}

// Buscar un valor en el árbol
ptrNodoArbol buscarNodo(ptrNodoArbol raiz, int valor) {
    // Caso base: árbol vacío o valor encontrado
    if (raiz == NULL || raiz->dato == valor) {
        return raiz;
    }

    // Si el valor es menor, buscar en el subárbol izquierdo
    if (valor < raiz->dato) {
        return buscarNodo(raiz->izquierdo, valor);
    }

    // Si el valor es mayor, buscar en el subárbol derecho
    return buscarNodo(raiz->derecho, valor);
}

// Encontrar el nodo con el valor mínimo (útil para eliminación)
static ptrNodoArbol encontrarNodoMinimo(ptrNodoArbol nodo) {
    ptrNodoArbol actual = nodo;

    // Descender hasta el nodo más a la izquierda
    while (actual && actual->izquierdo != NULL) {
        actual = actual->izquierdo;
    }

    return actual;
}

// Eliminar un nodo del árbol
ptrNodoArbol eliminarNodo(ptrNodoArbol raiz, int valor) {
    // Caso base: árbol vacío
    if (raiz == NULL) return raiz;

    // Buscar el nodo a eliminar
    if (valor < raiz->dato) {
        raiz->izquierdo = eliminarNodo(raiz->izquierdo, valor);
    }
    else if (valor > raiz->dato) {
        raiz->derecho = eliminarNodo(raiz->derecho, valor);
    }
    else {
        // Caso 1: Nodo sin hijos o con un solo hijo
        if (raiz->izquierdo == NULL) {
            ptrNodoArbol temp = raiz->derecho;
            free(raiz);
            return temp;
        }
        else if (raiz->derecho == NULL) {
            ptrNodoArbol temp = raiz->izquierdo;
            free(raiz);
            return temp;
        }

        // Caso 2: Nodo con dos hijos
        // Encontrar el sucesor inorden (mínimo en el subárbol derecho)
        ptrNodoArbol temp = encontrarNodoMinimo(raiz->derecho);

        // Copiar el valor del sucesor
        raiz->dato = temp->dato;

        // Eliminar el sucesor
        raiz->derecho = eliminarNodo(raiz->derecho, temp->dato);
    }
    return raiz;
}

// Recorrido inorden (izquierda-raíz-derecha)
void recorridoInorden(ptrNodoArbol raiz) {
    if (raiz == NULL) return;

    recorridoInorden(raiz->izquierdo);
    printf("%d ", raiz->dato);
    recorridoInorden(raiz->derecho);
}

// Recorrido preorden (raíz-izquierda-derecha)
void recorridoPreorden(ptrNodoArbol raiz) {
    if (raiz == NULL) return;

    printf("%d ", raiz->dato);
    recorridoPreorden(raiz->izquierdo);
    recorridoPreorden(raiz->derecho);
}

// Recorrido postorden (izquierda-derecha-raíz)
void recorridoPostorden(ptrNodoArbol raiz) {
    if (raiz == NULL) return;

    recorridoPostorden(raiz->izquierdo);
    recorridoPostorden(raiz->derecho);
    printf("%d ", raiz->dato);
}

// Liberar toda la memoria utilizada por el árbol
// void liberarArbol(ptrNodoArbol raiz) {
//     if (raiz == NULL) return;
//
//     // Primero liberar los subárboles (postorden)
//     liberarArbol(raiz->izquierdo);
//     liberarArbol(raiz->derecho);
//
//     // Luego liberar el nodo actual
//     free(raiz);
// }*/