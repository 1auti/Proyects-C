//
// Created by administrador on 5/26/25.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_VERTICES 100

// Estrucutura para nodos de lista adyacente
typedef  struct AdjNode {
    int vertice;
    int peso;
    struct AdjNode * siguiente;
}AdjNode;

// Estrucutura para lista de Adjayencia
typedef struct AdjList {
    AdjNode * raiz;
}AdjList;

// Estrucutura principal del grafo
typedef struct Graph {
    int numVertices;
    bool isDirected; // si es directo o no
    AdjList * adj_list; // para representar  con listas
    int adjMatrix[MAX_VERTICES][MAX_VERTICES]; //para representar una matrix
}Graph;

// Funciones de crear y destuir
Graph * crearGraph(int vertices, bool isDirected);
AdjNode* crearAdjNode(int vertice, int peso);

void destruirGraph(Graph * graph);

// Funciones para lista adyacencias
void removeEdgeList(Graph * graph, int raiz, int dest);
void printEdgeList(Graph * graph);

// Funciones para matriz de adyacencia
void addEdgeMatrix(Graph* graph, int src, int dest, int weight);
void removeEdgeMatrix(Graph* graph, int src, int dest);
void printGraphMatrix(Graph* graph);

// Funciones de busqueda
void DFS(Graph * graph, int initVertice);
void BFS(Graph * graph, int initVertice);
void DFSUtil(Graph * graph, int vertice, bool visitado[]);

bool addVertex(Graph* graph, int vertex);
// Función para verificar si existe una arista
bool hasEdge(Graph* graph, int from, int to);
bool addEdgeList(Graph* graph, int src, int dest, int weight);
//ALgoritmos avanzados
 //kruskal
 //bellman_ford
 //cycle_detection
 //dijkstra


#endif //GRAPH_H
