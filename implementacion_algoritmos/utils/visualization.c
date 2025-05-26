#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265359
#define INF INT_MAX

// Estructuras para visualización
typedef struct {
    double x, y;
} Point2D;

typedef struct {
    Point2D* positions;
    int numVertices;
    char** labels;
} GraphLayout;

// Prototipos
GraphLayout* createGraphLayout(int numVertices);
void freeGraphLayout(GraphLayout* layout);
void generateCircularLayout(GraphLayout* layout);
void generateRandomLayout(GraphLayout* layout, double width, double height);
void generateGridLayout(GraphLayout* layout, int cols, int rows);
void exportToSVG(const char* filename, int** graph, int numVertices, GraphLayout* layout);
void exportToDOT(const char* filename, int** graph, int numVertices, char** vertexNames, bool isDirected);
void exportToCSV(const char* filename, int** graph, int numVertices, char** vertexNames);
void printASCIIGraph(int** graph, int numVertices, char** vertexNames);
void printAdjacencyList(int** graph, int numVertices, char** vertexNames);
void visualizePathASCII(int* path, int pathLength, char** vertexNames);
void generateGraphStatistics(int** graph, int numVertices);

// Crear layout de grafo
GraphLayout* createGraphLayout(int numVertices) {
    GraphLayout* layout = (GraphLayout*)malloc(sizeof(GraphLayout));
    layout->positions = (Point2D*)malloc(numVertices * sizeof(Point2D));
    layout->numVertices = numVertices;
    layout->labels = (char**)malloc(numVertices * sizeof(char*));
    
    // Inicializar etiquetas por defecto
    for (int i = 0; i < numVertices; i++) {
        layout->labels[i] = (char*)malloc(10 * sizeof(char));
        sprintf(layout->labels[i], "%d", i);
    }
    
    return layout;
}

// Liberar layout
void freeGraphLayout(GraphLayout* layout) {
    if (!layout) return;
    
    if (layout->positions) free(layout->positions);
    if (layout->labels) {
        for (int i = 0; i < layout->numVertices; i++) {
            free(layout->labels[i]);
        }
        free(layout->labels);
    }
    free(layout);
}

// Layout circular
void generateCircularLayout(GraphLayout* layout) {
    double centerX = 200, centerY = 200, radius = 150;
    
    for (int i = 0; i < layout->numVertices; i++) {
        double angle = 2 * PI * i / layout->numVertices;
        layout->positions[i].x = centerX + radius * cos(angle);
        layout->positions[i].y = centerY + radius * sin(angle);
    }
}

// Layout aleatorio
void generateRandomLayout(GraphLayout* layout, double width, double height) {
    srand(42); // Seed fijo para reproducibilidad
    
    for (int i = 0; i < layout->numVertices; i++) {
        layout->positions[i].x = 50 + (rand() % (int)(width - 100));
        layout->positions[i].y = 50 + (rand() % (int)(height - 100));
    }
}

// Layout en grilla
void generateGridLayout(GraphLayout* layout, int cols, int rows) {
    double spacingX = 300.0 / cols;
    double spacingY = 300.0 / rows;
    
    for (int i = 0; i < layout->numVertices && i < cols * rows; i++) {
        int row = i / cols;
        int col = i % cols;
        layout->positions[i].x = 50 + col * spacingX;
        layout->positions[i].y = 50 + row * spacingY;
    }
}

// Exportar a SVG
void exportToSVG(const char* filename, int** graph, int numVertices, GraphLayout* layout) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: No se puede crear %s\n", filename);
        return;
    }
    
    // Header SVG
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<svg width=\"400\" height=\"400\" xmlns=\"http://www.w3.org/2000/svg\">\n");
    fprintf(file, "<style>\n");
    fprintf(file, ".vertex { fill: lightblue; stroke: black; stroke-width: 2; }\n");
    fprintf(file, ".edge { stroke: gray; stroke-width: 1; }\n");
    fprintf(file, ".label { font-family: Arial; font-size: 12px; text-anchor: middle; }\n");
    fprintf(file, "</style>\n\n");
    
    // Dibujar aristas
    fprintf(file, "<!-- Aristas -->\n");
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {
            if (graph[i][j] != 0) {
                fprintf(file, "<line class=\"edge\" x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\"/>\n",
                        layout->positions[i].x, layout->positions[i].y,
                        layout->positions[j].x, layout->positions[j].y);
                
                // Etiqueta del peso
                double midX = (layout->positions[i].x + layout->positions[j].x) / 2;
                double midY = (layout->positions[i].y + layout->positions[j].y) / 2;
                fprintf(file, "<text x=\"%.1f\" y=\"%.1f\" class=\"label\" fill=\"red\">%d</text>\n",
                        midX, midY - 5, graph[i][j]);
            }
        }
    }
    
    // Dibujar vértices
    fprintf(file, "\n<!-- Vértices -->\n");
    for (int i = 0; i < numVertices; i++) {
        fprintf(file, "<circle class=\"vertex\" cx=\"%.1f\" cy=\"%.1f\" r=\"15\"/>\n",
                layout->positions[i].x, layout->positions[i].y);
        fprintf(file, "<text x=\"%.1f\" y=\"%.1f\" class=\"label\">%s</text>\n",
                layout->positions[i].x, layout->positions[i].y + 4, layout->labels[i]);
    }
    
    fprintf(file, "</svg>\n");
    fclose(file);
    
    printf("Grafo exportado a SVG: %s\n", filename);
}

// Exportar a formato DOT (Graphviz)
void exportToDOT(const char* filename, int** graph, int numVertices, char** vertexNames, bool isDirected) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: No se puede crear %s\n", filename);
        return;
    }
    
    // Header DOT
    if (isDirected) {
        fprintf(file, "digraph G {\n");
    } else {
        fprintf(file, "graph G {\n");
    }
    
    fprintf(file, "    node [shape=circle, style=filled, fillcolor=lightblue];\n");
    fprintf(file, "    edge [color=gray];\n\n");
    
    // Declarar nodos
    for (int i = 0; i < numVertices; i++) {
        if (vertexNames && vertexNames[i]) {
            fprintf(file, "    %d [label=\"%s\"];\n", i, vertexNames[i]);
        } else {
            fprintf(file, "    %d;\n", i);
        }
    }
    
    fprintf(file, "\n");
    
    // Definir aristas
    for (int i = 0; i < numVertices; i++) {
        for (int j = (isDirected ? 0 : i + 1); j < numVertices; j++) {
            if (i != j && graph[i][j] != 0) {
                char connector = isDirected ? '>' : '-';
                fprintf(file, "    %d -%c %d [label=\"%d\"];\n", i, connector, j, graph[i][j]);
            }
        }
    }
    
    fprintf(file, "}\n");
    fclose(file);
    
    printf("Grafo exportado a DOT: %s\n", filename);
    printf("Para visualizar: dot -Tpng %s -o graph.png\n", filename);
}

// Exportar datos a CSV
void exportToCSV(const char* filename, int** graph, int numVertices, char** vertexNames) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: No se puede crear %s\n", filename);
        return;
    }
    
    // Header
    fprintf(file, "Origen,Destino,Peso\n");
    
    // Datos de aristas
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            if (graph[i][j] != 0) {
                if (vertexNames) {
                    fprintf(file, "%s,%s,%d\n", vertexNames[i], vertexNames[j], graph[i][j]);
                } else {
                    fprintf(file, "%d,%d,%d\n", i, j, graph[i][j]);
                }
            }
        }
    }
    
    fclose(file);
    printf("Datos exportados a CSV: %s\n", filename);
}

// Visualización ASCII simple
void printASCIIGraph(int** graph, int numVertices, char** vertexNames) {
    printf("\n🎨 VISUALIZACIÓN ASCII DEL GRAFO\n");
    printf("=================================\n");
    
    // Matriz de adyacencia formateada
    printf("Matriz de adyacencia:\n");
    printf("     ");
    for (int i = 0; i < numVertices; i++) {
        if (vertexNames && vertexNames[i]) {
            printf("%4s", vertexNames[i]);
        } else {
            printf("%4d", i);
        }
    }
    printf("\n");
    
    for (int i = 0; i < numVertices; i++) {
        if (vertexNames && vertexNames[i]) {
            printf("%4s:", vertexNames[i]);
        } else {
            printf("%4d:", i);
        }
        
        for (int j = 0; j < numVertices; j++) {
            if (graph[i][j] == 0) {
                printf("   .");
            } else {
                printf("%4d", graph[i][j]);
            }
        }
        printf("\n");
    }
}

// Lista de adyacencia formateada
void printAdjacencyList(int** graph, int numVertices, char** vertexNames) {
    printf("\n📋 LISTA DE ADYACENCIA\n");
    printf("======================\n");
    
    for (int i = 0; i < numVertices; i++) {
        if (vertexNames && vertexNames[i]) {
            printf("%s: ", vertexNames[i]);
        } else {
            printf("%d: ", i);
        }
        
        bool first = true;
        for (int j = 0; j < numVertices; j++) {
            if (graph[i][j] != 0) {
                if (!first) printf(", ");
                
                if (vertexNames && vertexNames[j]) {
                    printf("%s(%d)", vertexNames[j], graph[i][j]);
                } else {
                    printf("%d(%d)", j, graph[i][j]);
                }
                first = false;
            }
        }
        
        if (first) {
            printf("(sin conexiones)");
        }
        printf("\n");
    }
}

// Visualizar camino en ASCII
void visualizePathASCII(int* path, int pathLength, char** vertexNames) {
    printf("\n🛤️  CAMINO ENCONTRADO\n");
    printf("====================\n");
    
    for (int i = 0; i < pathLength; i++) {
        if (vertexNames && vertexNames[path[i]]) {
            printf("%s", vertexNames[path[i]]);
        } else {
            printf("%d", path[i]);
        }
        
        if (i < pathLength - 1) {
            printf(" → ");
        }
    }
    printf("\n");
    printf("Longitud del camino: %d saltos\n", pathLength - 1);
}

// Generar estadísticas del grafo
void generateGraphStatistics(int** graph, int numVertices) {
    printf("\n📊 ESTADÍSTICAS DEL GRAFO\n");
    printf("==========================\n");
    
    int numEdges = 0;
    int totalWeight = 0;
    int maxWeight = 0, minWeight = INT_MAX;
    int maxDegree = 0, minDegree = numVertices;
    
    // Contar aristas y calcular estadísticas
    for (int i = 0; i < numVertices; i++) {
        int degree = 0;
        for (int j = 0; j < numVertices; j++) {
            if (graph[i][j] != 0) {
                if (i <= j) { // Evitar contar aristas dos veces en grafos no dirigidos
                    numEdges++;
                    totalWeight += graph[i][j];
                    
                    if (graph[i][j] > maxWeight) maxWeight = graph[i][j];
                    if (graph[i][j] < minWeight) minWeight = graph[i][j];
                }
                degree++;
            }
        }
        
        if (degree > maxDegree) maxDegree = degree;
        if (degree < minDegree) minDegree = degree;
    }
    
    // Calcular densidad
    int maxPossibleEdges = numVertices * (numVertices - 1) / 2;
    double density = (double)numEdges / maxPossibleEdges;
    
    // Mostrar estadísticas
    printf("Número de vértices: %d\n", numVertices);
    printf("Número de aristas: %d\n", numEdges);
    printf("Densidad: %.3f (%.1f%%)\n", density, density * 100);
    printf("Grado promedio: %.2f\n", (double)(2 * numEdges) / numVertices);
    printf("Grado máximo: %d\n", maxDegree);
    printf("Grado mínimo: %d\n", minDegree);
    
    if (numEdges > 0) {
        printf("Peso total: %d\n", totalWeight);
        printf("Peso promedio: %.2f\n", (double)totalWeight / numEdges);
        printf("Peso máximo: %d\n", maxWeight);
        printf("Peso mínimo: %d\n", minWeight == INT_MAX ? 0 : minWeight);
    }
    
    // Clasificación del grafo
    printf("\nClasificación:\n");
    if (density < 0.1) {
        printf("  - Grafo disperso (baja densidad)\n");
    } else if (density > 0.7) {
        printf("  - Grafo denso (alta densidad)\n");
    } else {
        printf("  - Grafo de densidad media\n");
    }
    
    if (maxDegree - minDegree > numVertices / 2) {
        printf("  - Distribución de grados irregular\n");
    } else {
        printf("  - Distribución de grados regular\n");
    }
}

// Función de prueba
void testVisualization() {
    printf("🎨 PRUEBA DE VISUALIZACIÓN\n");
    printf("==========================\n");
    
    // Crear grafo de ejemplo
    int numVertices = 5;
    int** graph = (int**)malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        graph[i] = (int*)calloc(numVertices, sizeof(int));
    }
    
    // Llenar con datos
    graph[0][1] = graph[1][0] = 3;
    graph[0][2] = graph[2][0] = 2;
    graph[1][3] = graph[3][1] = 4;
    graph[2][4] = graph[4][2] = 1;
    graph[3][4] = graph[4][3] = 5;
    
    char* names[] = {"A", "B", "C", "D", "E"};
    
    // Crear layout y generar visualizaciones
    GraphLayout* layout = createGraphLayout(numVertices);
    for (int i = 0; i < numVertices; i++) {
        strcpy(layout->labels[i], names[i]);
    }
    
    generateCircularLayout(layout);
    
    // Exportar en diferentes formatos
    exportToSVG("graph_example.svg", graph, numVertices, layout);
    exportToDOT("graph_example.dot", graph, numVertices, names, false);
    exportToCSV("graph_example.csv", graph, numVertices, names);
    
    // Mostrar visualizaciones ASCII
    printASCIIGraph(graph, numVertices, names);
    printAdjacencyList(graph, numVertices, names);
    generateGraphStatistics(graph, numVertices);
    
    // Simular camino
    int path[] = {0, 1, 3, 4};
    visualizePathASCII(path, 4, names);
    
    // Limpiar memoria
    for (int i = 0; i < numVertices; i++) {
        free(graph[i]);
    }
    free(graph);
    freeGraphLayout(layout);
}