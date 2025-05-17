//
// Created by Lautaro on 13/05/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbol.h"

BTreeNode * crearNodo(bool es_hoja) {
    BTreeNode * nodo = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (nodo != NULL) {
        nodo->es_hoja = es_hoja;
        nodo->num_claves = 0;

        for (int i = 0 ; i < ORDER ; i++) {
            nodo->hijos[i] = NULL;
        }
    }

    return nodo;
}

// Funcion para inicializar un arbol B
BTree* inicializar() {
    BTree* arbol = (BTree*)malloc(sizeof(BTree));
    if (arbol == NULL) {
        printf("Error: No se pudo asignar memoria para el árbol\n");
        exit(1);
    }

    // Crear un nodo raíz como hoja (true)
    arbol->raiz = crearNodo(true);

    return arbol;
}

int buscarPosicion(BTreeNode * nodo, int posicion) {
    int i = 0;
    while (i < nodo->num_claves && posicion > nodo->claves[i]) {
        i++;
    }
    return i;
}


// Función para dividir un nodo hijo y ajustar el nodo padre
void dividirHijo(BTreeNode* padre, int indice, BTreeNode* hijo) {
    // Crear un nuevo nodo que será el hermano derecho
    BTreeNode* nuevo_nodo = crearNodo(hijo->es_hoja);
    nuevo_nodo->num_claves = ORDER/2 - 1;  // Para un árbol de orden impar

    // Copiar las claves superiores al nuevo nodo
    for (int j = 0; j < ORDER/2 - 1; j++) {
        nuevo_nodo->claves[j] = hijo->claves[j + ORDER/2];
        nuevo_nodo->registros[j] = hijo->registros[j + ORDER/2];
    }

    // Si no es hoja, también mover los punteros a hijos
    if (!hijo->es_hoja) {
        for (int j = 0; j < ORDER/2; j++) {
            nuevo_nodo->hijos[j] = hijo->hijos[j + ORDER/2];
        }
    }

    // Ajustar el número de claves en el hijo original
    hijo->num_claves = ORDER/2 - 1;

    // Hacer espacio en el padre para el nuevo hijo
    for (int j = padre->num_claves; j > indice; j--) {
        padre->hijos[j+1] = padre->hijos[j];
    }

    // Vincular el nuevo nodo como hijo del padre
    padre->hijos[indice+1] = nuevo_nodo;

    // Mover las claves en el padre para insertar la clave mediana
    for (int j = padre->num_claves-1; j >= indice; j--) {
        padre->claves[j+1] = padre->claves[j];
        padre->registros[j+1] = padre->registros[j];
    }

    // Copiar la clave mediana del hijo al padre
    padre->claves[indice] = hijo->claves[ORDER/2 - 1];
    padre->registros[indice] = hijo->registros[ORDER/2 - 1];
    padre->num_claves++;
}

// Insertar una clave en un nodo que no está completo
void insertarEnNodoNoLleno(BTreeNode* nodo, int clave, void* registro) {
    // Iniciar desde el extremo derecho
    int i = nodo->num_claves - 1;

    // Si es una hoja, simplemente insertamos en la posición correcta
    if (nodo->es_hoja) {
        // Desplazar elementos mayores que la clave
        while (i >= 0 && clave < nodo->claves[i]) {
            nodo->claves[i+1] = nodo->claves[i];
            nodo->registros[i+1] = nodo->registros[i];
            i--;
        }

        // Insertar la nueva clave y registro
        nodo->claves[i+1] = clave;
        nodo->registros[i+1] = registro;
        nodo->num_claves++;
    }
    else {
        // Encontrar el hijo donde debe ir la clave
        while (i >= 0 && clave < nodo->claves[i]) {
            i--;
        }
        i++;

        // Si ese hijo está lleno, hay que dividirlo
        if (nodo->hijos[i]->num_claves == ORDER-1) {
            dividirHijo(nodo, i, nodo->hijos[i]);

            // Después de dividir, la clave mediana está en nodo->claves[i]
            if (clave > nodo->claves[i]) {
                i++;
            }
        }
        insertarEnNodoNoLleno(nodo->hijos[i], clave, registro);
    }
}

// Insertar un registro en el árbol B (creando un índice)
void insertar(BTree* arbol, int clave, void* registro) {
    BTreeNode* raiz = arbol->raiz;

    // Si la raíz está llena, el árbol crece en altura
    if (raiz->num_claves == ORDER-1) {
        // Crear una nueva raíz
        BTreeNode* nueva_raiz = crearNodo(false);
        nueva_raiz->hijos[0] = raiz;
        arbol->raiz = nueva_raiz;

        // Dividir la antigua raíz y mover una clave a la nueva raíz
        dividirHijo(nueva_raiz, 0, raiz);

        // Insertar en el subárbol apropiado
        insertarEnNodoNoLleno(nueva_raiz, clave, registro);
    }
    else {
        // Si la raíz no está llena, simplemente insertar
        insertarEnNodoNoLleno(raiz, clave, registro);
    }
}

// Buscar un registro por su clave
void* buscar(BTreeNode* nodo, int clave) {
    // Encontrar la primera clave mayor o igual a la buscada
    int i = 0;
    while (i < nodo->num_claves && clave > nodo->claves[i]) {
        i++;
    }

    // Si encontramos la clave
    if (i < nodo->num_claves && clave == nodo->claves[i]) {
        return nodo->registros[i];
    }

    // Si no encontramos la clave y es un nodo hoja, no existe
    if (nodo->es_hoja) {
        return NULL;
    }

    // Buscar en el hijo apropiado
    return buscar(nodo->hijos[i], clave);
}

// Buscar en el árbol B
void* buscarEnArbol(BTree* arbol, int clave) {
    if (!arbol->raiz) {
        return NULL;
    }
    return buscar(arbol->raiz, clave);
}

// Liberar la memoria de todo el árbol
void liberarNodo(BTreeNode* nodo) {
    if (!nodo) return;

    // Si no es hoja, liberar primero los hijos
    if (!nodo->es_hoja) {
        for (int i = 0; i <= nodo->num_claves; i++) {
            liberarNodo(nodo->hijos[i]);
        }
    }

    // Liberar el nodo actual
    free(nodo);
}

void liberarArbol(BTree* arbol) {
    liberarNodo(arbol->raiz);
    free(arbol);
}

// Imprimir el árbol (para depuración)
void imprimirNodo(BTreeNode* nodo, int nivel) {
    if (!nodo) return;

    // Indentación según el nivel
    for (int i = 0; i < nivel; i++) {
        printf("  ");
    }

    // Imprimir las claves del nodo
    printf("Claves: ");
    for (int i = 0; i < nodo->num_claves; i++) {
        printf("%d ", nodo->claves[i]);
    }
    printf("\n");

    // Imprimir los hijos recursivamente
    if (!nodo->es_hoja) {
        for (int i = 0; i <= nodo->num_claves; i++) {
            imprimirNodo(nodo->hijos[i], nivel + 1);
        }
    }
}

void imprimirArbol(BTree* arbol) {
    printf("Estructura del árbol B:\n");
    imprimirNodo(arbol->raiz, 0);
    printf("\n");
}

