cmake_minimum_required(VERSION 3.16)
project(navegacion_gps C)

set(CMAKE_C_STANDARD 99)

# Incluir directorios
include_directories(.)
include_directories(algoritmos)
include_directories(estructura_datos)
include_directories(graph)
include_directories(utils)
include_directories(navegacion_gps)

# Archivos del proyecto GPS
set(GPS_SOURCES
        navegacion_gps/gps_system.c
        navegacion_gps/main_gps.c

        # Algoritmos necesarios
        algoritmos/dijkstra.c
        algoritmos/bellman_ford.c
        algoritmos/dfs_bfs.c
        algoritmos/cycle_detection.c

        # Estructuras de datos
        estructura_datos/hash_map.c
        estructura_datos/priority_queue.c
        estructura_datos/union_find.c

        # Grafo
        graph/graph.c
)

# Crear ejecutable
add_executable(gps_sistema ${GPS_SOURCES})

# Enlazar librería matemática
target_link_libraries(gps_sistema m)

# Flags de compilación
target_compile_options(gps_sistema PRIVATE -Wall -Wextra -g)

# Mensaje de información
message(STATUS "Configurando proyecto GPS con:")
message(STATUS "  - Soporte para algoritmos de grafos")
message(STATUS "  - Hash maps y colas de prioridad")
message(STATUS "  - Sistema de navegación completo")
message(STATUS "  - Librería matemática enlazada")