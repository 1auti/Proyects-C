# Makefile para el Sistema de Análisis de Redes Sociales

CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lm

# Directorios
SOCIAL_DIR = social_network
BUILD_DIR = build

# Incluir paths de tu proyecto existente
INCLUDES = -I. \
-I./graph \
-I./algoritmos \
-I./estructura_datos \
-I./utils \
-I./$(SOCIAL_DIR)

# Archivos fuente necesarios del proyecto existente
GRAPH_SRCS = graph/graph.c \
graph/lista_adyacente.c \
graph/matrix_adyacente.c

ALGO_SRCS = algoritmos/dijkstra.c \
algoritmos/bellman_ford.c \
algoritmos/dfs_bfs.c \
algoritmos/cycle_detection.c \
algoritmos/kruskal.c

DS_SRCS = estructura_datos/union_find.c \
estructura_datos/priority_queue.c \
estructura_datos/hash_map.c

UTIL_SRCS = utils/file_io.c \
utils/visualization.c

# Archivos del sistema de redes sociales
SOCIAL_SRCS = $(SOCIAL_DIR)/social_network.c \
$(SOCIAL_DIR)/social_network_demo.c

# Todos los archivos fuente
ALL_SRCS = $(GRAPH_SRCS) $(ALGO_SRCS) $(DS_SRCS) $(UTIL_SRCS) $(SOCIAL_SRCS)

# Archivos objeto
OBJS = $(ALL_SRCS:%.c=$(BUILD_DIR)/%.o)

# Ejecutable
TARGET = social_network_analyzer

# Regla principal
all: $(BUILD_DIR) $(TARGET)

# Crear directorio de build
$(BUILD_DIR):
mkdir -p $(BUILD_DIR)
mkdir -p $(BUILD_DIR)/graph
mkdir -p $(BUILD_DIR)/algoritmos
mkdir -p $(BUILD_DIR)/estructura_datos
mkdir -p $(BUILD_DIR)/utils
mkdir -p $(BUILD_DIR)/$(SOCIAL_DIR)

# Compilar ejecutable
$(TARGET): $(OBJS)
$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
@echo "✅ Compilación exitosa: $(TARGET)"

# Regla para compilar archivos objeto
$(BUILD_DIR)/%.o: %.c
$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar
clean:
rm -rf $(BUILD_DIR) $(TARGET)
@echo "🧹 Limpieza completada"

# Ejecutar
run: $(TARGET)
./$(TARGET)

# Compilar y ejecutar
test: clean all run

# Ayuda
help:
@echo "📋 Comandos disponibles:"
@echo "  make all    - Compilar el proyecto"
@echo "  make clean  - Limpiar archivos compilados"
@echo "  make run    - Ejecutar el programa"
@echo "  make test   - Limpiar, compilar y ejecutar"
@echo "  make help   - Mostrar esta ayuda"

.PHONY: all clean run test help