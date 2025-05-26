#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

// Incluir los algoritmos a medir
// #include "../algoritmos/dijkstra.h"
// #include "../algoritmos/bellman_ford.h"
// #include "../algoritmos/kruskal.h"
// #include "../algoritmos/dfs_bfs.h"

typedef struct {
    char algorithmName[50];
    double executionTime;
    int graphSize;
    int numEdges;
    bool success;
} BenchmarkResult;

typedef struct {
    BenchmarkResult* results;
    int numResults;
    int capacity;
} BenchmarkSuite;

// Prototipos
BenchmarkSuite* createBenchmarkSuite();
void addBenchmarkResult(BenchmarkSuite* suite, const char* algName, double time, int size, int edges, bool success);
double getCurrentTime();
int** generateRandomGraph(int numVertices, double density, int maxWeight);
int** generateCompleteGraph(int numVertices, int maxWeight);
int** generateSparseGraph(int numVertices, int avgDegree, int maxWeight);
void benchmarkAlgorithm(BenchmarkSuite* suite, const char* algName, void (*algorithm)(), int** graph, int numVertices);
void runCompleteBenchmark();
void printBenchmarkResults(BenchmarkSuite* suite);
void saveBenchmarkResults(BenchmarkSuite* suite, const char* filename);
void freeBenchmarkSuite(BenchmarkSuite* suite);

// Obtener tiempo actual en microsegundos
double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// Crear suite de benchmarks
BenchmarkSuite* createBenchmarkSuite() {
    BenchmarkSuite* suite = (BenchmarkSuite*)malloc(sizeof(BenchmarkSuite));
    suite->capacity = 100;
    suite->results = (BenchmarkResult*)malloc(suite->capacity * sizeof(BenchmarkResult));
    suite->numResults = 0;
    return suite;
}

// Agregar resultado de benchmark
void addBenchmarkResult(BenchmarkSuite* suite, const char* algName, double time, int size, int edges, bool success) {
    if (suite->numResults >= suite->capacity) {
        suite->capacity *= 2;
        suite->results = (BenchmarkResult*)realloc(suite->results, suite->capacity * sizeof(BenchmarkResult));
    }
    
    BenchmarkResult* result = &suite->results[suite->numResults];
    strncpy(result->algorithmName, algName, 49);
    result->algorithmName[49] = '\0';
    result->executionTime = time;
    result->graphSize = size;
    result->numEdges = edges;
    result->success = success;
    
    suite->numResults++;
}

// Generar grafo aleatorio
int** generateRandomGraph(int numVertices, double density, int maxWeight) {
    int** graph = (int**)malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        graph[i] = (int*)calloc(numVertices, sizeof(int));
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {
            if ((double)rand() / RAND_MAX < density) {
                int weight = 1 + rand() % maxWeight;
                graph[i][j] = graph[j][i] = weight;
            }
        }
    }
    
    return graph;
}

// Generar grafo completo
int** generateCompleteGraph(int numVertices, int maxWeight) {
    int** graph = (int**)malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        graph[i] = (int*)calloc(numVertices, sizeof(int));
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {
            int weight = 1 + rand() % maxWeight;
            graph[i][j] = graph[j][i] = weight;
        }
    }
    
    return graph;
}

// Generar grafo disperso
int** generateSparseGraph(int numVertices, int avgDegree, int maxWeight) {
    int** graph = (int**)malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        graph[i] = (int*)calloc(numVertices, sizeof(int));
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < numVertices; i++) {
        int degree = avgDegree + (rand() % 3) - 1; // ±1 variación
        degree = degree < 1 ? 1 : degree;
        degree = degree >= numVertices ? numVertices - 1 : degree;
        
        for (int d = 0; d < degree; d++) {
            int j = rand() % numVertices;
            if (i != j && graph[i][j] == 0) {
                int weight = 1 + rand() % maxWeight;
                graph[i][j] = graph[j][i] = weight;
            }
        }
    }
    
    return graph;
}

// Benchmark de un algoritmo específico
void benchmarkDijkstraWrapper(int** graph, int numVertices) {
    // PathResult* result = dijkstra(graph, numVertices, 0, numVertices-1);
    // if (result) freePathResult(result);
    
    // Simulación para el ejemplo
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            volatile int temp = graph[i][j];
            (void)temp; // Evitar warning de variable no usada
        }
    }
}

void benchmarkBellmanFordWrapper(int** graph, int numVertices) {
    // PathResult* result = bellmanFord(graph, numVertices, 0, numVertices-1);
    // if (result) freePathResult(result);
    
    // Simulación más costosa
    for (int k = 0; k < numVertices; k++) {
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                volatile int temp = graph[i][j];
                (void)temp;
            }
        }
    }
}

void benchmarkKruskalWrapper(int** graph, int numVertices) {
    // MST* mst = kruskalMST(graph, numVertices);
    // if (mst) freeMST(mst);
    
    // Simulación de ordenamiento
    for (int i = 0; i < numVertices * numVertices; i++) {
        volatile int temp = i * i;
        (void)temp;
    }
}

// Medir tiempo de ejecución de algoritmo
void benchmarkAlgorithm(BenchmarkSuite* suite, const char* algName, void (*algorithm)(int**, int), int** graph, int numVertices) {
    // Contar aristas
    int numEdges = 0;
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {
            if (graph[i][j] != 0) numEdges++;
        }
    }
    
    printf("Midiendo %s con %d vértices, %d aristas... ", algName, numVertices, numEdges);
    
    double startTime = getCurrentTime();
    
    // Ejecutar algoritmo
    algorithm(graph, numVertices);
    
    double endTime = getCurrentTime();
    double executionTime = (endTime - startTime) / 1000.0; // Convertir a milisegundos
    
    printf("%.3f ms\n", executionTime);
    
    addBenchmarkResult(suite, algName, executionTime, numVertices, numEdges, true);
}

// Ejecutar suite completa de benchmarks
void runCompleteBenchmark() {
    printf("🏃‍♂️ EJECUTANDO BENCHMARKS COMPLETOS\n");
    printf("====================================\n");
    
    BenchmarkSuite* suite = createBenchmarkSuite();
    
    int sizes[] = {10, 25, 50, 100, 200};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < numSizes; s++) {
        int size = sizes[s];
        printf("\n📊 Probando con %d vértices:\n", size);
        
        // Grafo disperso (baja densidad)
        printf("  Grafo disperso (densidad 0.1):\n");
        int** sparseGraph = generateRandomGraph(size, 0.1, 10);
        benchmarkAlgorithm(suite, "Dijkstra-Sparse", benchmarkDijkstraWrapper, sparseGraph, size);
        benchmarkAlgorithm(suite, "BellmanFord-Sparse", benchmarkBellmanFordWrapper, sparseGraph, size);
        benchmarkAlgorithm(suite, "Kruskal-Sparse", benchmarkKruskalWrapper, sparseGraph, size);
        
        // Grafo denso (alta densidad)
        printf("  Grafo denso (densidad 0.7):\n");
        int** denseGraph = generateRandomGraph(size, 0.7, 10);
        benchmarkAlgorithm(suite, "Dijkstra-Dense", benchmarkDijkstraWrapper, denseGraph, size);
        benchmarkAlgorithm(suite, "BellmanFord-Dense", benchmarkBellmanFordWrapper, denseGraph, size);
        benchmarkAlgorithm(suite, "Kruskal-Dense", benchmarkKruskalWrapper, denseGraph, size);
        
        // Limpiar memoria
        for (int i = 0; i < size; i++) {
            free(sparseGraph[i]);
            free(denseGraph[i]);
        }
        free(sparseGraph);
        free(denseGraph);
    }
    
    // Mostrar y guardar resultados
    printBenchmarkResults(suite);
    saveBenchmarkResults(suite, "benchmark_results.csv");
    
    freeBenchmarkSuite(suite);
}

// Imprimir resultados de benchmarks
void printBenchmarkResults(BenchmarkSuite* suite) {
    printf("\n📈 RESULTADOS DE BENCHMARKS\n");
    printf("============================\n");
    
    printf("%-20s %-10s %-10s %-12s %-8s\n", 
           "Algoritmo", "Vértices", "Aristas", "Tiempo (ms)", "Estado");
    printf("%-20s %-10s %-10s %-12s %-8s\n", 
           "--------------------", "----------", "----------", "------------", "--------");
    
    for (int i = 0; i < suite->numResults; i++) {
        BenchmarkResult* r = &suite->results[i];
        printf("%-20s %-10d %-10d %-12.3f %-8s\n",
               r->algorithmName, r->graphSize, r->numEdges, r->executionTime,
               r->success ? "OK" : "FAIL");
    }
    
    // Estadísticas por algoritmo
    printf("\n📊 ESTADÍSTICAS POR ALGORITMO:\n");
    
    char algorithms[][30] = {"Dijkstra-Sparse", "Dijkstra-Dense", "BellmanFord-Sparse", 
                            "BellmanFord-Dense", "Kruskal-Sparse", "Kruskal-Dense"};
    int numAlgs = sizeof(algorithms) / sizeof(algorithms[0]);
    
    for (int a = 0; a < numAlgs; a++) {
        double totalTime = 0;
        int count = 0;
        double minTime = 1e9, maxTime = 0;
        
        for (int i = 0; i < suite->numResults; i++) {
            if (strcmp(suite->results[i].algorithmName, algorithms[a]) == 0) {
                totalTime += suite->results[i].executionTime;
                count++;
                if (suite->results[i].executionTime < minTime) {
                    minTime = suite->results[i].executionTime;
                }
                if (suite->results[i].executionTime > maxTime) {
                    maxTime = suite->results[i].executionTime;
                }
            }
        }
        
        if (count > 0) {
            printf("  %s:\n", algorithms[a]);
            printf("    Promedio: %.3f ms\n", totalTime / count);
            printf("    Mínimo: %.3f ms\n", minTime);
            printf("    Máximo: %.3f ms\n", maxTime);
            printf("    Muestras: %d\n\n", count);
        }
    }
}

// Guardar resultados en CSV
void saveBenchmarkResults(BenchmarkSuite* suite, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: No se puede crear %s\n", filename);
        return;
    }
    
    // Header CSV
    fprintf(file, "Algoritmo,Vertices,Aristas,Tiempo_ms,Estado\n");
    
    // Datos
    for (int i = 0; i < suite->numResults; i++) {
        BenchmarkResult* r = &suite->results[i];
        fprintf(file, "%s,%d,%d,%.3f,%s\n",
                r->algorithmName, r->graphSize, r->numEdges, r->executionTime,
                r->success ? "OK" : "FAIL");
    }
    
    fclose(file);
    printf("Resultados guardados en %s\n", filename);
}

// Liberar memoria del benchmark suite
void freeBenchmarkSuite(BenchmarkSuite* suite) {
    if (suite) {
        if (suite->results) free(suite->results);
        free(suite);
    }
}

// Función de prueba
void testBenchmarks() {
    printf("⏱️  PRUEBA DE BENCHMARKS\n");
    printf("=======================\n");
    
    // Ejecutar benchmarks con grafos pequeños
    BenchmarkSuite* suite = createBenchmarkSuite();
    
    int** testGraph = generateRandomGraph(20, 0.3, 5);
    
    benchmarkAlgorithm(suite, "Test-Dijkstra", benchmarkDijkstraWrapper, testGraph, 20);
    benchmarkAlgorithm(suite, "Test-BellmanFord", benchmarkBellmanFordWrapper, testGraph, 20);
    benchmarkAlgorithm(suite, "Test-Kruskal", benchmarkKruskalWrapper, testGraph, 20);
    
    printBenchmarkResults(suite);
    
    // Limpiar
    for (int i = 0; i < 20; i++) {
        free(testGraph[i]);
    }
    free(testGraph);
    freeBenchmarkSuite(suite);
}