#include "union_find.h"

// Crear estructura Union-Find
UnionFind* createUnionFind(int n) {
    if (n <= 0) return NULL;
    
    UnionFind* uf = (UnionFind*)malloc(sizeof(UnionFind));
    if (!uf) return NULL;
    
    uf->parent = (int*)malloc(n * sizeof(int));
    uf->rank = (int*)malloc(n * sizeof(int));
    uf->size = (int*)malloc(n * sizeof(int));
    
    if (!uf->parent || !uf->rank || !uf->size) {
        destroyUnionFind(uf);
        return NULL;
    }
    
    uf->totalElements = n;
    uf->numSets = n;
    
    // Inicializar: cada elemento es su propio conjunto
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
        uf->size[i] = 1;
    }
    
    return uf;
}

// Crear conjunto individual
void makeSet(UnionFind* uf, int x) {
    if (!uf || x < 0 || x >= uf->totalElements) return;
    
    uf->parent[x] = x;
    uf->rank[x] = 0;
    uf->size[x] = 1;
}

// Encontrar representante con compresión de caminos
int find(UnionFind* uf, int x) {
    if (!uf || x < 0 || x >= uf->totalElements) return -1;
    
    if (uf->parent[x] != x) {
        uf->parent[x] = find(uf, uf->parent[x]); // Compresión de caminos
    }
    return uf->parent[x];
}

// Encontrar sin compresión (para debugging)
int findWithoutCompression(UnionFind* uf, int x) {
    if (!uf || x < 0 || x >= uf->totalElements) return -1;
    
    while (uf->parent[x] != x) {
        x = uf->parent[x];
    }
    return x;
}

// Unión por rango
bool unionByRank(UnionFind* uf, int x, int y) {
    if (!uf) return false;
    
    int rootX = find(uf, x);
    int rootY = find(uf, y);
    
    if (rootX == -1 || rootY == -1) return false;
    if (rootX == rootY) return false; // Ya están en el mismo conjunto
    
    // Unión por rango
    if (uf->rank[rootX] < uf->rank[rootY]) {
        uf->parent[rootX] = rootY;
        uf->size[rootY] += uf->size[rootX];
    } else if (uf->rank[rootX] > uf->rank[rootY]) {
        uf->parent[rootY] = rootX;
        uf->size[rootX] += uf->size[rootY];
    } else {
        uf->parent[rootY] = rootX;
        uf->size[rootX] += uf->size[rootY];
        uf->rank[rootX]++;
    }
    
    uf->numSets--;
    return true;
}

// Unión por tamaño
bool unionBySize(UnionFind* uf, int x, int y) {
    if (!uf) return false;
    
    int rootX = find(uf, x);
    int rootY = find(uf, y);
    
    if (rootX == -1 || rootY == -1) return false;
    if (rootX == rootY) return false; // Ya están en el mismo conjunto
    
    // Unión por tamaño (el más pequeño apunta al más grande)
    if (uf->size[rootX] < uf->size[rootY]) {
        uf->parent[rootX] = rootY;
        uf->size[rootY] += uf->size[rootX];
    } else {
        uf->parent[rootY] = rootX;
        uf->size[rootX] += uf->size[rootY];
    }
    
    uf->numSets--;
    return true;
}

// Verificar si dos elementos están conectados
bool areConnected(UnionFind* uf, int x, int y) {
    if (!uf) return false;
    
    int rootX = find(uf, x);
    int rootY = find(uf, y);
    
    return (rootX != -1 && rootY != -1 && rootX == rootY);
}

// Obtener tamaño del conjunto
int getSetSize(UnionFind* uf, int x) {
    if (!uf) return -1;
    
    int root = find(uf, x);
    return (root != -1) ? uf->size[root] : -1;
}

// Obtener número de conjuntos
int getNumSets(UnionFind* uf) {
    return uf ? uf->numSets : -1;
}

// Obtener miembros de un conjunto
int* getSetMembers(UnionFind* uf, int x, int* setSize) {
    if (!uf || !setSize) return NULL;
    
    int root = find(uf, x);
    if (root == -1) {
        *setSize = 0;
        return NULL;
    }
    
    *setSize = uf->size[root];
    int* members = (int*)malloc(*setSize * sizeof(int));
    int memberCount = 0;
    
    // Encontrar todos los elementos que pertenecen al mismo conjunto
    for (int i = 0; i < uf->totalElements; i++) {
        if (find(uf, i) == root) {
            members[memberCount++] = i;
        }
    }
    
    return members;
}

// Imprimir estado del Union-Find
void printUnionFind(UnionFind* uf) {
    if (!uf) {
        printf("Union-Find es NULL\n");
        return;
    }
    
    printf("\n=== UNION-FIND ===\n");
    printf("Elementos totales: %d\n", uf->totalElements);
    printf("Número de conjuntos: %d\n", uf->numSets);
    
    printf("\nElemento -> Padre (Rango, Tamaño):\n");
    for (int i = 0; i < uf->totalElements; i++) {
        printf("%d -> %d (rango: %d, tamaño: %d)\n", 
               i, uf->parent[i], uf->rank[i], uf->size[i]);
    }
    
    printf("\nConjuntos:\n");
    bool* visited = (bool*)calloc(uf->totalElements, sizeof(bool));
    
    for (int i = 0; i < uf->totalElements; i++) {
        int root = find(uf, i);
        if (!visited[root]) {
            visited[root] = true;
            printf("Conjunto %d: { ", root);
            
            for (int j = 0; j < uf->totalElements; j++) {
                if (find(uf, j) == root) {
                    printf("%d ", j);
                }
            }
            printf("} (tamaño: %d)\n", uf->size[root]);
        }
    }
    
    free(visited);
    printf("==================\n");
}

// Destruir Union-Find
void destroyUnionFind(UnionFind* uf) {
    if (!uf) return;
    
    if (uf->parent) free(uf->parent);
    if (uf->rank) free(uf->rank);
    if (uf->size) free(uf->size);
    free(uf);
}