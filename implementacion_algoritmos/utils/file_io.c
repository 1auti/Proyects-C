#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Estructuras para manejo de archivos
typedef struct {
    int** matrix;
    int numVertices;
    bool isDirected;
    char** vertexNames;
} GraphData;

// Prototipos
GraphData* loadGraphFromFile(const char* filename);
bool saveGraphToFile(const char* filename, int** graph, int numVertices, bool isDirected, char** vertexNames);
bool loadAdjacencyMatrix(const char* filename, int*** graph, int* numVertices);
bool saveAdjacencyMatrix(const char* filename, int** graph, int numVertices);
bool loadEdgeList(const char* filename, int*** graph, int* numVertices);
bool saveEdgeList(const char* filename, int** graph, int numVertices);
void freeGraphData(GraphData* data);

// Cargar grafo desde archivo (formato automático)
GraphData* loadGraphFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se puede abrir el archivo %s\n", filename);
        return NULL;
    }
    
    GraphData* data = (GraphData*)malloc(sizeof(GraphData));
    data->matrix = NULL;
    data->vertexNames = NULL;
    data->numVertices = 0;
    data->isDirected = false;
    
    char line[1024];
    
    // Leer primera línea para determinar formato
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        free(data);
        return NULL;
    }
    
    // Reiniciar archivo
    fseek(file, 0, SEEK_SET);
    
    // Detectar formato basado en primera línea
    if (strstr(line, "VERTICES") || strstr(line, "vertices")) {
        // Formato personalizado con metadata
        fscanf(file, "VERTICES: %d\n", &data->numVertices);
        
        char directed[10];
        fscanf(file, "DIRECTED: %s\n", directed);
        data->isDirected = (strcmp(directed, "true") == 0 || strcmp(directed, "1") == 0);
        
        // Leer nombres de vértices si existen
        if (fgets(line, sizeof(line), file) && strstr(line, "NAMES:")) {
            data->vertexNames = (char**)malloc(data->numVertices * sizeof(char*));
            for (int i = 0; i < data->numVertices; i++) {
                data->vertexNames[i] = (char*)malloc(50 * sizeof(char));
                fscanf(file, "%s", data->vertexNames[i]);
            }
        }
        
        // Leer matriz
        data->matrix = (int**)malloc(data->numVertices * sizeof(int*));
        for (int i = 0; i < data->numVertices; i++) {
            data->matrix[i] = (int*)malloc(data->numVertices * sizeof(int));
            for (int j = 0; j < data->numVertices; j++) {
                fscanf(file, "%d", &data->matrix[i][j]);
            }
        }
    } else {
        // Formato matriz simple
        fscanf(file, "%d", &data->numVertices);
        
        data->matrix = (int**)malloc(data->numVertices * sizeof(int*));
        for (int i = 0; i < data->numVertices; i++) {
            data->matrix[i] = (int*)malloc(data->numVertices * sizeof(int));
            for (int j = 0; j < data->numVertices; j++) {
                fscanf(file, "%d", &data->matrix[i][j]);
            }
        }
    }
    
    fclose(file);
    printf("Grafo cargado exitosamente: %d vértices\n", data->numVertices);
    return data;
}

// Guardar grafo en archivo
bool saveGraphToFile(const char* filename, int** graph, int numVertices, bool isDirected, char** vertexNames) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: No se puede crear el archivo %s\n", filename);
        return false;
    }
    
    // Escribir metadata
    fprintf(file, "VERTICES: %d\n", numVertices);
    fprintf(file, "DIRECTED: %s\n", isDirected ? "true" : "false");
    
    // Escribir nombres si existen
    if (vertexNames) {
        fprintf(file, "NAMES: ");
        for (int i = 0; i < numVertices; i++) {
            fprintf(file, "%s ", vertexNames[i]);
        }
        fprintf(file, "\n");
    }
    
    fprintf(file, "MATRIX:\n");
    
    // Escribir matriz de adyacencia
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            fprintf(file, "%d ", graph[i][j]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("Grafo guardado exitosamente en %s\n", filename);
    return true;
}

// Cargar solo matriz de adyacencia
bool loadAdjacencyMatrix(const char* filename, int*** graph, int* numVertices) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    fscanf(file, "%d", numVertices);
    
    *graph = (int**)malloc(*numVertices * sizeof(int*));
    for (int i = 0; i < *numVertices; i++) {
        (*graph)[i] = (int*)malloc(*numVertices * sizeof(int));
        for (int j = 0; j < *numVertices; j++) {
            fscanf(file, "%d", &(*graph)[i][j]);
        }
    }
    
    fclose(file);
    return true;
}

// Guardar solo matriz de adyacencia
bool saveAdjacencyMatrix(const char* filename, int** graph, int numVertices) {
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "%d\n", numVertices);
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            fprintf(file, "%d ", graph[i][j]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    return true;
}

// Cargar desde lista de aristas
bool loadEdgeList(const char* filename, int*** graph, int* numVertices) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    int numEdges;
    fscanf(file, "%d %d", numVertices, &numEdges);
    
    // Inicializar matriz
    *graph = (int**)malloc(*numVertices * sizeof(int*));
    for (int i = 0; i < *numVertices; i++) {
        (*graph)[i] = (int*)calloc(*numVertices, sizeof(int));
    }
    
    // Leer aristas
    for (int i = 0; i < numEdges; i++) {
        int src, dest, weight;
        fscanf(file, "%d %d %d", &src, &dest, &weight);
        (*graph)[src][dest] = weight;
    }
    
    fclose(file);
    return true;
}

// Guardar como lista de aristas
bool saveEdgeList(const char* filename, int** graph, int numVertices) {
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    // Contar aristas
    int numEdges = 0;
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            if (graph[i][j] != 0) numEdges++;
        }
    }
    
    fprintf(file, "%d %d\n", numVertices, numEdges);
    
    // Escribir aristas
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            if (graph[i][j] != 0) {
                fprintf(file, "%d %d %d\n", i, j, graph[i][j]);
            }
        }
    }
    
    fclose(file);
    return true;
}

// Liberar memoria de GraphData
void freeGraphData(GraphData* data) {
    if (!data) return;
    
    if (data->matrix) {
        for (int i = 0; i < data->numVertices; i++) {
            free(data->matrix[i]);
        }
        free(data->matrix);
    }
    
    if (data->vertexNames) {
        for (int i = 0; i < data->numVertices; i++) {
            free(data->vertexNames[i]);
        }
        free(data->vertexNames);
    }
    
    free(data);
}

// Función de ejemplo/test
void testFileIO() {
    printf("🗂️  PRUEBA DE FILE I/O\n");
    printf("====================\n");
    
    // Crear grafo de ejemplo
    int numVertices = 4;
    int** graph = (int**)malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        graph[i] = (int*)calloc(numVertices, sizeof(int));
    }
    
    // Llenar con datos de ejemplo
    graph[0][1] = 5;
    graph[0][2] = 3;
    graph[1][3] = 2;
    graph[2][3] = 4;
    
    char* names[] = {"A", "B", "C", "D"};
    
    // Guardar grafo
    saveGraphToFile("test_graph.txt", graph, numVertices, false, names);
    saveAdjacencyMatrix("test_matrix.txt", graph, numVertices);
    saveEdgeList("test_edges.txt", graph, numVertices);
    
    // Cargar grafo
    GraphData* loaded = loadGraphFromFile("test_graph.txt");
    if (loaded) {
        printf("Grafo cargado: %d vértices\n", loaded->numVertices);
        freeGraphData(loaded);
    }
    
    // Limpiar
    for (int i = 0; i < numVertices; i++) {
        free(graph[i]);
    }
    free(graph);
}