//
// Created by Lautaro on 13/05/2025.
//

#ifndef ARBOL_H
#define ARBOL_H
#define ORDER 5 //numero maximo de  hijos que puede tener un nodo
#include <stdbool.h>

//EL dato
typedef struct RegistroE {
    int id; //clave primaria
    char nombre[50];
    float saldo;
}RegistroE;

// Estrucutura de un nodo del arbol B
typedef struct BTreeNode {
    // un array de los ids
    int claves[ORDER-1]; // se considera el 0 por eso el -1
    void * registros[ORDER-1]; // creamos un puntero de los datos
    struct BTreeNode * hijos[ORDER]; // punteros a hijos nodos
    int num_claves; // numeros de claves
    bool es_hoja; // SI es verdadero  o no
}BTreeNode;

// Estructura de un  arbol B
typedef struct{
 BTreeNode * raiz;
}BTree;

BTreeNode * crearNodo(bool es_hoja);
BTree * inicializar();
int buscarPosicion(BTreeNode * nodo, int posicion);
void dividirHijo(BTreeNode * padre, int indice, BTreeNode * hijo);
void insertarEnNodoNoLleno(BTreeNode* nodo, int clave, void* registro);
void insertar(BTree* arbol, int clave, void* registro);
void* buscar(BTreeNode* nodo, int clave);
void* buscarEnArbol(BTree* arbol, int clave);
void liberarNodo(BTreeNode* nodo);
void liberarArbol(BTree* arbol);
void imprimirNodo(BTreeNode* nodo, int nivel);
void imprimirArbol(BTree* arbol);














#endif //ARBOL_H
