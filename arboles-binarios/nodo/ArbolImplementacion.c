/*#include <stdio.h>
#include "nodo.h"

int nodoImplementacion() {
    ptrNodoArbol raiz = NULL;

    // Insertar valores en el árbol
    raiz = insertarNodo(raiz, 50);
    insertarNodo(raiz, 30);
    insertarNodo(raiz, 20);
    insertarNodo(raiz, 40);
    insertarNodo(raiz, 70);
    insertarNodo(raiz, 60);
    insertarNodo(raiz, 80);

    // Mostrar árbol en diferentes recorridos
    printf("Recorrido Inorden: ");
    recorridoInorden(raiz);
    printf("\n");

    printf("Recorrido Preorden: ");
    recorridoPreorden(raiz);
    printf("\n");

    printf("Recorrido Postorden: ");
    recorridoPostorden(raiz);
    printf("\n");

    // Buscar un valor
    int valorBuscado = 40;
    ptrNodoArbol encontrado = buscarNodo(raiz, valorBuscado);
    if (encontrado != NULL) {
        printf("Valor %d encontrado en el arbol\n", valorBuscado);
    } else {
        printf("Valor %d NO encontrado en el arbol\n", valorBuscado);
    }

    // Eliminar un nodo
    int valorEliminar = 30;
    raiz = eliminarNodo(raiz, valorEliminar);
    printf("Arbol despues de eliminar %d: ", valorEliminar);
    recorridoInorden(raiz);
    printf("\n");

    // Liberar memoria
    liberarArbol(raiz);

    return 0;
}*/