//
// Created by Lautaro on 12/05/2025.
//

#ifndef NODO_ARBOL_H
#define NODO_ARBOL_H

// Definición de la estructura para un nodo de árbol binario
typedef struct NodoArbol {
    int dato;
    struct NodoArbol* izquierdo;
    struct NodoArbol* derecho;
} NodoArbol;

// Tipo de puntero a nodo (opcional pero útil)
typedef NodoArbol* ptrNodoArbol;

// Prototipos de funciones
// ptrNodoArbol crearNodo(int valor);
ptrNodoArbol insertarNodo(ptrNodoArbol raiz, int valor);
ptrNodoArbol buscarNodo(ptrNodoArbol raiz, int valor);
ptrNodoArbol eliminarNodo(ptrNodoArbol raiz, int valor);
void recorridoInorden(ptrNodoArbol raiz);
void recorridoPreorden(ptrNodoArbol raiz);
void recorridoPostorden(ptrNodoArbol raiz);
// void liberarArbol(ptrNodoArbol raiz);

#endif //NODO_ARBOL_H