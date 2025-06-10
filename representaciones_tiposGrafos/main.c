#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ucontext.h>

typedef struct  Nodo{
int vertices;
    int pesos;
    struct Nodo * siguiente;
}Nodo;

typedef struct GrafoLista {
    Nodo ** listas;
    int vertices;
    bool dirigido;
    bool ponderado;
}GrafoLista;

typedef struct GrafoMatriz {
    Nodo ** matriz;
    int vertices;
    bool dirigido;
    bool ponderado;
}GrafoMatriz;

// FUnciones auxillares estas funciones nos van a ayudar a la creacion de los grafos , monejo de los mismos

GrafoLista * crear_grafo_lista(int vertices, bool dirigido, bool ponderado) {
    GrafoLista * grafo = malloc(sizeof(GrafoLista));
    grafo->dirigido = dirigido;
    grafo->vertices = vertices;
    grafo->ponderado = ponderado;

    //Inicializamos los vaolres adentro de la lista
    grafo->listas = malloc(vertices * sizeof(Nodo*));
    for(int i = 0 ; i < vertices ; i++) {
        grafo->listas[i] = NULL;
    }
    return  grafo;
}

GrafoMatriz * crearGrafoMatriz(int vertices, bool dirigido, bool ponderado) {
    GrafoMatriz * grafo_matriz = malloc(sizeof(GrafoMatriz));
    grafo_matriz->dirigido = dirigido;
    grafo_matriz->ponderado = ponderado;
    grafo_matriz->vertices = vertices;

    //Inicializamos el la matriz del grafo
    grafo_matriz->matriz = malloc(vertices * sizeof(int*));
    for(int i = 0 ; i < 0; i++) {
        /*
         * El calloc se utiliza para una asignacion de memoria continua y lo inicializa en 0
         */
        grafo_matriz->matriz[i] = calloc(vertices,sizeof(int));
    }
    return  grafo_matriz;
}

Nodo * crear_nodo(int vertices, int peso) {
    Nodo * nuevo = malloc(sizeof(Nodo));
    nuevo->vertices = vertices;
    nuevo->pesos = peso;
    nuevo->siguiente = NULL;
    return nuevo;
}

void agergarAristaMatriz(GrafoMatriz * grafo, int origen , int destino, int peso) {
    grafo->matriz[origen][destino].pesos  = peso;
    if(!grafo->dirigido) {
        grafo->matriz[destino][origen].pesos = peso;
    }
}

void agregarAristaLista(GrafoLista * grafo_lista, int origen, int destino, int peso) {
    grafo_lista->listas[origen][destino].pesos = peso;
    if(!grafo_lista ->dirigido) {
        grafo_lista->listas[destino][origen].pesos = peso;
    }
}

// ALgoritmos para detectar propiedades

// Realiza una busqueda en profundiadd (DFS) desde un nodo inicio, marcando que nodos han sido visitiados.
// Es una funcion auxillar para verificar si el grafo esta conectado (conexo)
bool es_conexion_DFS(GrafoLista * grafo_lista, int inicio, bool * visitado) {
    visitado[inicio] = true;

    Nodo * actual = grafo_lista->listas[inicio];
    while (actual != NULL) {
        if(!visitado[actual->vertices]) {
            es_conexion_DFS(grafo_lista,actual->vertices, visitado);
        }
        actual = actual->siguiente;
    }
    return  true;
}

// Verifica que sea un conexo
bool verificarConexidad(GrafoLista * grafo_lista) {
    if(grafo_lista->dirigido) return false;

    bool * visitado = calloc(grafo_lista->vertices, sizeof(bool));
    // Verifica que los nodos estan conectados
    es_conexion_DFS(grafo_lista,0,visitado);

    for(int i = 0 ; i < grafo_lista->vertices; i++) {
        if(!visitado[i]) {
            free(visitado);
            return false;
        }
    }

    free(visitado);
    return true;

}
//Verifica si se puede divir el nodo en dos conjuntos tal que no hay aristas entre los nodos del mismo conjunto
bool esBipartito(GrafoLista* grafo) {
    //
    int* color = malloc(grafo->vertices * sizeof(int));
    for (int i = 0; i < grafo->vertices; i++) {
        color[i] = -1; // No coloreado
    }

    // Cola para BFS
    int* cola = malloc(grafo->vertices * sizeof(int));

    for (int inicio = 0; inicio < grafo->vertices; inicio++) {
        if (color[inicio] == -1) {
            int frente = 0, final = 0;
            color[inicio] = 0;
            cola[final++] = inicio;

            while (frente < final) {
                int u = cola[frente++];

                Nodo* actual = grafo->listas[u];
                while (actual != NULL) {
                    int v = actual->vertices;

                    if (color[v] == -1) {
                        color[v] = 1 - color[u]; // Color opuesto
                        cola[final++] = v;
                    } else if (color[v] == color[u]) {
                        free(color);
                        free(cola);
                        return false; // No es bipartito
                    }
                    actual = actual->siguiente;
                }
            }
        }
    }

    free(color);
    free(cola);
    return true;
}

// ========================================
// DEMOSTRACIÓN: UNA REPRESENTACIÓN, MÚLTIPLES TIPOS
// ========================================

void demostracionRepresentacionesVsTipos() {
    printf("=== REPRESENTACIONES vs TIPOS DE GRAFOS ===\n\n");

    printf("CONCEPTO CLAVE:\n");
    printf("- REPRESENTACIÓN = CÓMO almacenamos los datos\n");
    printf("- TIPO/PROPIEDAD = QUÉ características tiene el grafo\n\n");

    printf("¡PUEDES usar CUALQUIER representación para CUALQUIER tipo de grafo!\n\n");

    // ========================================
    // GRAFO CONEXO
    // ========================================
    printf("1. GRAFO CONEXO (todos los vértices alcanzables):\n");
    printf("   Representación: Lista de Adyacencia\n");

    GrafoLista* grafoConexo = crear_grafo_lista(4, false, false);
    agregarAristaLista(grafoConexo, 0, 1, 1);
    agregarAristaLista(grafoConexo, 1, 2, 1);
    agregarAristaLista(grafoConexo, 2, 3, 1);
    agregarAristaLista(grafoConexo, 3, 0, 1);

    printf("   Aristas: (0-1), (1-2), (2-3), (3-0)\n");
    printf("   ¿Es conexo? SÍ - puedes llegar de cualquier vértice a otro\n\n");

    // ========================================
    // GRAFO NO CONEXO
    // ========================================
    printf("2. GRAFO NO CONEXO (componentes separados):\n");
    printf("   Representación: Matriz de Adyacencia\n");

    GrafoMatriz* grafoNoConexo = crearGrafoMatriz(4, false, false);
    agergarAristaMatriz(grafoNoConexo, 0, 1, 1);  // Componente 1: {0,1}
   agergarAristaMatriz(grafoNoConexo, 2, 3, 1);  // Componente 2: {2,3}

    printf("   Aristas: (0-1), (2-3)\n");
    printf("   ¿Es conexo? NO - hay 2 componentes separados\n");
    printf("   Matriz de adyacencia:\n");
    for (int i = 0; i < 4; i++) {
        printf("   ");
        for (int j = 0; j < 4; j++) {
            printf("%d ", grafoNoConexo->matriz[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    // ========================================
    // GRAFO BIPARTITO
    // ========================================
    printf("3. GRAFO BIPARTITO (vértices en 2 conjuntos):\n");
    printf("   Representación: Lista de Adyacencia\n");

    GrafoLista* grafoBipartito = crear_grafo_lista(6, false, false);
    // Conjunto A: {0, 1, 2}, Conjunto B: {3, 4, 5}
    // Solo conexiones entre conjuntos diferentes
    agregarAristaLista(grafoBipartito, 0, 3, 1);
    agregarAristaLista(grafoBipartito, 0, 4, 1);
    agregarAristaLista(grafoBipartito, 1, 4, 1);
    agregarAristaLista(grafoBipartito, 1, 5, 1);
    agregarAristaLista(grafoBipartito, 2, 5, 1);

    printf("   Conjunto A: {0, 1, 2}\n");
    printf("   Conjunto B: {3, 4, 5}\n");
    printf("   Aristas solo entre conjuntos diferentes\n");
    printf("   ¿Es bipartito? SÍ\n\n");

    // ========================================
    // GRAFO COMPLETO
    // ========================================
    printf("4. GRAFO COMPLETO (todos conectados con todos):\n");
    printf("   Representación: Matriz de Adyacencia\n");

    GrafoMatriz* grafoCompleto = crearGrafoMatriz(4, false, false);
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            agergarAristaMatriz(grafoCompleto, i, j, 1);
        }
    }

    printf("   Todas las aristas posibles: (0-1), (0-2), (0-3), (1-2), (1-3), (2-3)\n");
    printf("   Total aristas: %d (máximo para %d vértices)\n", 6, 4);
    printf("   ¿Es completo? SÍ\n\n");
}

// ========================================
// TABLA COMPARATIVA
// ========================================

void tablaComparativa() {
    printf("=== TABLA COMPARATIVA ===\n\n");

    printf("REPRESENTACIÓN vs TIPOS DE GRAFOS:\n");
    printf("┌─────────────────────┬─────────────────┬─────────────────┐\n");
    printf("│ TIPO DE GRAFO       │ LISTA ADYACENC. │ MATRIZ ADYACENC.│\n");
    printf("├─────────────────────┼─────────────────┼─────────────────┤\n");
    printf("│ Dirigido            │       ✅        │       ✅        │\n");
    printf("│ No Dirigido         │       ✅        │       ✅        │\n");
    printf("│ Conexo              │       ✅        │       ✅        │\n");
    printf("│ No Conexo           │       ✅        │       ✅        │\n");
    printf("│ Bipartito           │       ✅        │       ✅        │\n");
    printf("│ Completo            │       ✅        │       ✅        │\n");
    printf("│ Planar              │       ✅        │       ✅        │\n");
    printf("│ Cíclico             │       ✅        │       ✅        │\n");
    printf("│ Árbol               │       ✅        │       ✅        │\n");
    printf("│ Ponderado           │       ✅        │       ✅        │\n");
    printf("└─────────────────────┴─────────────────┴─────────────────┘\n\n");

    printf("¡TODAS las representaciones pueden manejar TODOS los tipos!\n\n");

    printf("La elección de representación depende de:\n");
    printf("• Densidad del grafo (disperso vs denso)\n");
    printf("• Operaciones más frecuentes\n");
    printf("• Restricciones de memoria\n");
    printf("• Algoritmos que vas a implementar\n\n");
}

// ========================================
// EJEMPLOS DEL MUNDO REAL
// ========================================

void ejemplosDelMundoReal() {
    printf("=== EJEMPLOS DEL MUNDO REAL ===\n\n");

    printf("1. RED SOCIAL (Facebook):\n");
    printf("   • Tipo: No dirigido, conexo (idealmente), no planar\n");
    printf("   • Representación recomendada: Lista de adyacencia\n");
    printf("   • Por qué: Grafo disperso, necesitas recorrer amigos\n\n");

    printf("2. PÁGINAS WEB (Enlaces):\n");
    printf("   • Tipo: Dirigido, no conexo, no planar\n");
    printf("   • Representación recomendada: Lista de adyacencia\n");
    printf("   • Por qué: Muy disperso, cambios frecuentes\n\n");

    printf("3. MAPA DE CARRETERAS:\n");
    printf("   • Tipo: No dirigido, conexo, planar, ponderado\n");
    printf("   • Representación recomendada: Lista de adyacencia\n");
    printf("   • Por qué: Cada ciudad tiene pocas conexiones\n\n");

    printf("4. CIRCUITO INTEGRADO:\n");
    printf("   • Tipo: Dirigido, conexo, planar\n");
    printf("   • Representación recomendada: Matriz (si es denso)\n");
    printf("   • Por qué: Verificaciones rápidas de conexión\n\n");

    printf("5. TORNEO DEPORTIVO:\n");
    printf("   • Tipo: Dirigido, completo, no cíclico\n");
    printf("   • Representación recomendada: Matriz de adyacencia\n");
    printf("   • Por qué: Todos juegan contra todos, consultas rápidas\n");
}

// ========================================
// FUNCIÓN PRINCIPAL
// ========================================

int main() {
    printf("=== REPRESENTACIONES vs TIPOS DE GRAFOS ===\n\n");

    demostracionRepresentacionesVsTipos();

    printf("\n=== VERIFICACIÓN DE PROPIEDADES ===\n");

    // Crear un grafo para probar
    GrafoLista* grafoPrueba = crear_grafo_lista(4, false, false);
    agregarAristaLista(grafoPrueba, 0, 1, 1);
    agregarAristaLista(grafoPrueba, 1, 2, 1);
    agregarAristaLista(grafoPrueba, 2, 3, 1);
    agregarAristaLista(grafoPrueba, 3, 0, 1);

    printf("Grafo de prueba con aristas: (0-1), (1-2), (2-3), (3-0)\n");
    printf("¿Es conexo? %s\n", verificarConexidad(grafoPrueba) ? "SÍ" : "NO");
    printf("¿Es bipartito? %s\n", esBipartito(grafoPrueba) ? "SÍ" : "NO");

    tablaComparativa();
    ejemplosDelMundoReal();

    printf("\n=== CONCLUSIÓN ===\n");
    printf("✅ Lista y Matriz de adyacencia son REPRESENTACIONES\n");
    printf("✅ Conexo, planar, bipartito, etc. son PROPIEDADES\n");
    printf("✅ Cualquier representación puede manejar cualquier tipo\n");
    printf("✅ La elección depende de eficiencia, no de compatibilidad\n");

    return 0;
}


