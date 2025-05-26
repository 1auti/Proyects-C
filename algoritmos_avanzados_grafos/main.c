#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define INF INT_MAX

// ========================================
// ESTRUCTURAS DE DATOS
// ========================================

typedef struct Nodo {
    int vertice;
    int peso;
    struct Nodo* siguiente;
} Nodo;

typedef struct {
    Nodo** listas;
    int vertices;
    bool dirigido;
    bool ponderado;
} GrafoLista;

typedef struct {
    int origen, destino, peso;
} Arista;

// Estructura para Union-Find
typedef struct {
    int* padre;
    int* rango;
    int size;
} UnionFind;

// ========================================
// FUNCIONES AUXILIARES BÁSICAS
// ========================================

GrafoLista* crearGrafoLista(int vertices, bool dirigido, bool ponderado) {
    GrafoLista* grafo = malloc(sizeof(GrafoLista));
    grafo->vertices = vertices;
    grafo->dirigido = dirigido;
    grafo->ponderado = ponderado;

    grafo->listas = malloc(vertices * sizeof(Nodo*));
    for (int i = 0; i < vertices; i++) {
        grafo->listas[i] = NULL;
    }
    return grafo;
}

Nodo* crearNodo(int vertice, int peso) {
    Nodo* nuevo = malloc(sizeof(Nodo));
    nuevo->vertice = vertice;
    nuevo->peso = peso;
    nuevo->siguiente = NULL;
    return nuevo;
}

void agregarArista(GrafoLista* grafo, int origen, int destino, int peso) {
    Nodo* nuevo = crearNodo(destino, peso);
    nuevo->siguiente = grafo->listas[origen];
    grafo->listas[origen] = nuevo;

    if (!grafo->dirigido) {
        Nodo* nuevoInv = crearNodo(origen, peso);
        nuevoInv->siguiente = grafo->listas[destino];
        grafo->listas[destino] = nuevoInv;
    }
}

void mostrarGrafo(GrafoLista* grafo) {
    printf("\nGrafo (%s, %s):\n",
           grafo->dirigido ? "dirigido" : "no dirigido",
           grafo->ponderado ? "ponderado" : "no ponderado");

    for (int i = 0; i < grafo->vertices; i++) {
        printf("Vértice %d: ", i);
        Nodo* actual = grafo->listas[i];
        while (actual != NULL) {
            if (grafo->ponderado) {
                printf("(%d, peso:%d) ", actual->vertice, actual->peso);
            } else {
                printf("%d ", actual->vertice);
            }
            actual = actual->siguiente;
        }
        printf("\n");
    }
}

// ========================================
// 1. DETECCIÓN DE CICLOS
// ========================================

// DFS para detectar ciclos en grafo NO dirigido
bool dfsDetectarCicloNoDirigido(GrafoLista* grafo, int vertice, bool* visitado, int padre) {
    visitado[vertice] = true;

    Nodo* actual = grafo->listas[vertice];
    while (actual != NULL) {
        if (!visitado[actual->vertice]) {
            if (dfsDetectarCicloNoDirigido(grafo, actual->vertice, visitado, vertice)) {
                return true;
            }
        } else if (actual->vertice != padre) {
            return true; // Ciclo encontrado
        }
        actual = actual->siguiente;
    }
    return false;
}

// DFS para detectar ciclos en grafo DIRIGIDO (usando colores)
bool dfsDetectarCicloDirigido(GrafoLista* grafo, int vertice, int* color) {
    color[vertice] = 1; // Gris (en proceso)

    Nodo* actual = grafo->listas[vertice];
    while (actual != NULL) {
        if (color[actual->vertice] == 1) {
            return true; // Back edge - ciclo encontrado
        }
        if (color[actual->vertice] == 0 &&
            dfsDetectarCicloDirigido(grafo, actual->vertice, color)) {
            return true;
        }
        actual = actual->siguiente;
    }

    color[vertice] = 2; // Negro (procesado)
    return false;
}

bool tieneCiclo(GrafoLista* grafo) {
    if (grafo->dirigido) {
        // Grafo dirigido - usar colores
        int* color = calloc(grafo->vertices, sizeof(int)); // 0=blanco, 1=gris, 2=negro

        for (int i = 0; i < grafo->vertices; i++) {
            if (color[i] == 0) {
                if (dfsDetectarCicloDirigido(grafo, i, color)) {
                    free(color);
                    return true;
                }
            }
        }
        free(color);
        return false;
    } else {
        // Grafo no dirigido
        bool* visitado = calloc(grafo->vertices, sizeof(bool));

        for (int i = 0; i < grafo->vertices; i++) {
            if (!visitado[i]) {
                if (dfsDetectarCicloNoDirigido(grafo, i, visitado, -1)) {
                    free(visitado);
                    return true;
                }
            }
        }
        free(visitado);
        return false;
    }
}

// ========================================
// 2. COMPONENTES CONEXOS
// ========================================

void dfsComponente(GrafoLista* grafo, int vertice, bool* visitado, int* componente, int numComponente) {
    visitado[vertice] = true;
    componente[vertice] = numComponente;

    Nodo* actual = grafo->listas[vertice];
    while (actual != NULL) {
        if (!visitado[actual->vertice]) {
            dfsComponente(grafo, actual->vertice, visitado, componente, numComponente);
        }
        actual = actual->siguiente;
    }
}

int encontrarComponentesConexos(GrafoLista* grafo) {
    if (grafo->dirigido) {
        printf("Componentes conexos solo para grafos no dirigidos\n");
        return -1;
    }

    bool* visitado = calloc(grafo->vertices, sizeof(bool));
    int* componente = malloc(grafo->vertices * sizeof(int));
    int numComponentes = 0;

    for (int i = 0; i < grafo->vertices; i++) {
        if (!visitado[i]) {
            dfsComponente(grafo, i, visitado, componente, numComponentes);
            numComponentes++;
        }
    }

    printf("\nComponentes conexos encontrados: %d\n", numComponentes);
    for (int i = 0; i < numComponentes; i++) {
        printf("Componente %d: ", i);
        for (int j = 0; j < grafo->vertices; j++) {
            if (componente[j] == i) {
                printf("%d ", j);
            }
        }
        printf("\n");
    }

    free(visitado);
    free(componente);
    return numComponentes;
}

// ========================================
// 3. ORDENACIÓN TOPOLÓGICA
// ========================================

void dfsOrdenacionTopologica(GrafoLista* grafo, int vertice, bool* visitado, int* pila, int* indice) {
    visitado[vertice] = true;

    Nodo* actual = grafo->listas[vertice];
    while (actual != NULL) {
        if (!visitado[actual->vertice]) {
            dfsOrdenacionTopologica(grafo, actual->vertice, visitado, pila, indice);
        }
        actual = actual->siguiente;
    }

    pila[(*indice)++] = vertice;
}

void ordenacionTopologica(GrafoLista* grafo) {
    if (!grafo->dirigido) {
        printf("Ordenación topológica solo para grafos dirigidos\n");
        return;
    }

    if (tieneCiclo(grafo)) {
        printf("El grafo tiene ciclos - no se puede hacer ordenación topológica\n");
        return;
    }

    bool* visitado = calloc(grafo->vertices, sizeof(bool));
    int* pila = malloc(grafo->vertices * sizeof(int));
    int indice = 0;

    for (int i = 0; i < grafo->vertices; i++) {
        if (!visitado[i]) {
            dfsOrdenacionTopologica(grafo, i, visitado, pila, &indice);
        }
    }

    printf("\nOrdenación topológica: ");
    for (int i = indice - 1; i >= 0; i--) {
        printf("%d ", pila[i]);
    }
    printf("\n");

    free(visitado);
    free(pila);
}

// ========================================
// 4. ALGORITMO DE DIJKSTRA
// ========================================

int encontrarMinimaDistancia(int* distancia, bool* visitado, int vertices) {
    int min = INF, minIndex = -1;

    for (int v = 0; v < vertices; v++) {
        if (!visitado[v] && distancia[v] <= min) {
            min = distancia[v];
            minIndex = v;
        }
    }
    return minIndex;
}

void dijkstra(GrafoLista* grafo, int origen) {
    if (!grafo->ponderado) {
        printf("Dijkstra requiere grafo ponderado\n");
        return;
    }

    int* distancia = malloc(grafo->vertices * sizeof(int));
    bool* visitado = calloc(grafo->vertices, sizeof(bool));
    int* padre = malloc(grafo->vertices * sizeof(int));

    // Inicializar distancias
    for (int i = 0; i < grafo->vertices; i++) {
        distancia[i] = INF;
        padre[i] = -1;
    }
    distancia[origen] = 0;

    for (int count = 0; count < grafo->vertices - 1; count++) {
        int u = encontrarMinimaDistancia(distancia, visitado, grafo->vertices);
        if (u == -1) break; // No hay más vértices alcanzables

        visitado[u] = true;

        Nodo* actual = grafo->listas[u];
        while (actual != NULL) {
            int v = actual->vertice;
            int peso = actual->peso;

            if (!visitado[v] && distancia[u] != INF &&
                distancia[u] + peso < distancia[v]) {
                distancia[v] = distancia[u] + peso;
                padre[v] = u;
            }
            actual = actual->siguiente;
        }
    }

    printf("\nCaminos más cortos desde vértice %d:\n", origen);
    for (int i = 0; i < grafo->vertices; i++) {
        printf("Vértice %d: ", i);
        if (distancia[i] == INF) {
            printf("No alcanzable\n");
        } else {
            printf("Distancia = %d, Camino: ", distancia[i]);

            // Reconstruir camino
            int camino[grafo->vertices];
            int longitud = 0;
            int actual = i;

            while (actual != -1) {
                camino[longitud++] = actual;
                actual = padre[actual];
            }

            for (int j = longitud - 1; j >= 0; j--) {
                printf("%d", camino[j]);
                if (j > 0) printf(" -> ");
            }
            printf("\n");
        }
    }

    free(distancia);
    free(visitado);
    free(padre);
}

// ========================================
// 5. UNION-FIND PARA KRUSKAL
// ========================================

UnionFind* crearUnionFind(int n) {
    UnionFind* uf = malloc(sizeof(UnionFind));
    uf->padre = malloc(n * sizeof(int));
    uf->rango = calloc(n, sizeof(int));
    uf->size = n;

    for (int i = 0; i < n; i++) {
        uf->padre[i] = i; // Cada elemento es su propio padre
    }

    return uf;
}

int find(UnionFind* uf, int x) {
    if (uf->padre[x] != x) {
        uf->padre[x] = find(uf, uf->padre[x]); // Compresión de caminos
    }
    return uf->padre[x];
}

bool unionSets(UnionFind* uf, int x, int y) {
    int rootX = find(uf, x);
    int rootY = find(uf, y);

    if (rootX == rootY) {
        return false; // Ya están en el mismo conjunto
    }

    // Unión por rango
    if (uf->rango[rootX] < uf->rango[rootY]) {
        uf->padre[rootX] = rootY;
    } else if (uf->rango[rootX] > uf->rango[rootY]) {
        uf->padre[rootY] = rootX;
    } else {
        uf->padre[rootY] = rootX;
        uf->rango[rootX]++;
    }

    return true;
}

// ========================================
// 6. ALGORITMO DE KRUSKAL
// ========================================

int compararAristas(const void* a, const void* b) {
    Arista* aristaA = (Arista*)a;
    Arista* aristaB = (Arista*)b;
    return aristaA->peso - aristaB->peso;
}

void kruskal(GrafoLista* grafo) {
    if (grafo->dirigido) {
        printf("Kruskal solo para grafos no dirigidos\n");
        return;
    }

    if (!grafo->ponderado) {
        printf("Kruskal requiere grafo ponderado\n");
        return;
    }

    // Recopilar todas las aristas
    Arista* aristas = malloc(grafo->vertices * grafo->vertices * sizeof(Arista));
    int numAristas = 0;

    for (int i = 0; i < grafo->vertices; i++) {
        Nodo* actual = grafo->listas[i];
        while (actual != NULL) {
            if (i < actual->vertice) { // Evitar aristas duplicadas en grafo no dirigido
                aristas[numAristas].origen = i;
                aristas[numAristas].destino = actual->vertice;
                aristas[numAristas].peso = actual->peso;
                numAristas++;
            }
            actual = actual->siguiente;
        }
    }

    // Ordenar aristas por peso
    qsort(aristas, numAristas, sizeof(Arista), compararAristas);

    UnionFind* uf = crearUnionFind(grafo->vertices);

    printf("\nÁrbol de Expansión Mínima (Kruskal):\n");
    int pesoTotal = 0;
    int aristasEnMST = 0;

    for (int i = 0; i < numAristas && aristasEnMST < grafo->vertices - 1; i++) {
        if (unionSets(uf, aristas[i].origen, aristas[i].destino)) {
            printf("Arista: %d - %d, Peso: %d\n",
                   aristas[i].origen, aristas[i].destino, aristas[i].peso);
            pesoTotal += aristas[i].peso;
            aristasEnMST++;
        }
    }

    printf("Peso total del MST: %d\n", pesoTotal);

    free(aristas);
    free(uf->padre);
    free(uf->rango);
    free(uf);
}

// ========================================
// FUNCIÓN PRINCIPAL DE DEMOSTRACIÓN
// ========================================

int main() {
    printf("=== ALGORITMOS AVANZADOS DE GRAFOS ===\n");

    // ========================================
    // 1. DETECCIÓN DE CICLOS
    // ========================================
    printf("\n1. DETECCIÓN DE CICLOS\n");
    printf("----------------------\n");

    // Grafo no dirigido con ciclo
    GrafoLista* grafoNoCiclo = crearGrafoLista(4, false, false);
    agregarArista(grafoNoCiclo, 0, 1, 1);
    agregarArista(grafoNoCiclo, 1, 2, 1);
    agregarArista(grafoNoCiclo, 2, 3, 1);
    agregarArista(grafoNoCiclo, 3, 0, 1); // Crea un ciclo

    mostrarGrafo(grafoNoCiclo);
    printf("¿Tiene ciclo? %s\n", tieneCiclo(grafoNoCiclo) ? "SÍ" : "NO");

    // ========================================
    // 2. COMPONENTES CONEXOS
    // ========================================
    printf("\n2. COMPONENTES CONEXOS\n");
    printf("----------------------\n");

    GrafoLista* grafoDesconectado = crearGrafoLista(6, false, false);
    agregarArista(grafoDesconectado, 0, 1, 1);
    agregarArista(grafoDesconectado, 1, 2, 1);
    agregarArista(grafoDesconectado, 3, 4, 1);
    // Vértice 5 queda aislado

    mostrarGrafo(grafoDesconectado);
    encontrarComponentesConexos(grafoDesconectado);

    // ========================================
    // 3. ORDENACIÓN TOPOLÓGICA
    // ========================================
    printf("\n3. ORDENACIÓN TOPOLÓGICA\n");
    printf("------------------------\n");

    GrafoLista* grafoDAG = crearGrafoLista(6, true, false);
    agregarArista(grafoDAG, 5, 2, 1);
    agregarArista(grafoDAG, 5, 0, 1);
    agregarArista(grafoDAG, 4, 0, 1);
    agregarArista(grafoDAG, 4, 1, 1);
    agregarArista(grafoDAG, 2, 3, 1);
    agregarArista(grafoDAG, 3, 1, 1);

    mostrarGrafo(grafoDAG);
    ordenacionTopologica(grafoDAG);

    // ========================================
    // 4. DIJKSTRA
    // ========================================
    printf("\n4. ALGORITMO DE DIJKSTRA\n");
    printf("------------------------\n");

    GrafoLista* grafoPonderado = crearGrafoLista(5, false, true);
    agregarArista(grafoPonderado, 0, 1, 2);
    agregarArista(grafoPonderado, 0, 3, 6);
    agregarArista(grafoPonderado, 1, 2, 3);
    agregarArista(grafoPonderado, 1, 3, 8);
    agregarArista(grafoPonderado, 1, 4, 5);
    agregarArista(grafoPonderado, 2, 4, 7);
    agregarArista(grafoPonderado, 3, 4, 9);

    mostrarGrafo(grafoPonderado);
    dijkstra(grafoPonderado, 0);

    // ========================================
    // 5. KRUSKAL
    // ========================================
    printf("\n5. ALGORITMO DE KRUSKAL\n");
    printf("-----------------------\n");

    mostrarGrafo(grafoPonderado);
    kruskal(grafoPonderado);

    printf("\n=== FIN DE LA DEMOSTRACIÓN ===\n");
    return 0;
}