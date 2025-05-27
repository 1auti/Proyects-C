#include "kruskal.h"

// Comparador para ordenar aristas por peso
int compareEdges(const void* a, const void* b) {
    Edge* edgeA = (Edge*)a;
    Edge* edgeB = (Edge*)b;
    return edgeA->weight - edgeB->weight;
}

// Algoritmo de Kruskal para Árbol de Expansión Mínima
MST* kruskalMST(int** graph, int numVertices) {
    MST* mst = (MST*)malloc(sizeof(MST));
    mst->edges = (Edge*)malloc((numVertices - 1) * sizeof(Edge));
    mst->edgeCount = 0;
    mst->totalWeight = 0;

    // Crear lista de todas las aristas
    Edge* allEdges = (Edge*)malloc((numVertices * numVertices) * sizeof(Edge));
    int edgeCount = 0;

    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) { // Solo consideramos j > i para evitar duplicados
            if (graph[i][j] != 0) {
                allEdges[edgeCount].src = i;
                allEdges[edgeCount].dest = j;
                allEdges[edgeCount].weight = graph[i][j];
                edgeCount++;
            }
        }
    }

    // Ordenar aristas por peso
    qsort(allEdges, edgeCount, sizeof(Edge), compareEdges);

    // Crear Union-Find usando la función de union_find.h
    UnionFind* uf = createUnionFind(numVertices);

    // Procesar aristas en orden de peso
    for (int i = 0; i < edgeCount && mst->edgeCount < numVertices - 1; i++) {
        Edge currentEdge = allEdges[i];

        // Verificar si la arista crea un ciclo usando find() de union_find.h
        if (find(uf, currentEdge.src) != find(uf, currentEdge.dest)) {
            // Agregar arista al MST
            mst->edges[mst->edgeCount] = currentEdge;
            mst->totalWeight += currentEdge.weight;
            mst->edgeCount++;

            // Unir los conjuntos usando unionByRank() de union_find.h
            unionByRank(uf, currentEdge.src, currentEdge.dest);
        }
    }

    free(allEdges);
    destroyUnionFind(uf);  // Usar destroyUnionFind() de union_find.h

    return mst;
}

void printMST(MST* mst) {
    printf("\n=== ÁRBOL DE EXPANSIÓN MÍNIMA (KRUSKAL) ===\n");
    printf("Aristas en el MST:\n");

    for (int i = 0; i < mst->edgeCount; i++) {
        printf("  %d -- %d  (peso: %d)\n",
               mst->edges[i].src, mst->edges[i].dest, mst->edges[i].weight);
    }

    printf("Peso total del MST: %d\n", mst->totalWeight);
    printf("Número de aristas: %d\n", mst->edgeCount);
    printf("========================================\n");
}

void freeMST(MST* mst) {
    if (mst) {
        if (mst->edges) {
            free(mst->edges);
        }
        free(mst);
    }
}