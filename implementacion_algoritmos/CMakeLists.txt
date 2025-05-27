cmake_minimum_required(VERSION 3.10)
project(implementacion_algoritmos C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Incluir directorios de headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/graph
    ${CMAKE_CURRENT_SOURCE_DIR}/algoritmos
    ${CMAKE_CURRENT_SOURCE_DIR}/estructura_datos
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/navegacion_gps
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

# Fuentes comunes (sin archivos main)
set(GRAPH_SOURCES
    graph/graph.c
    graph/lista_adyacente.c
    graph/matrix_adyacente.c
)

set(ALGORITHM_SOURCES
    algoritmos/dijkstra.c
    algoritmos/bellman_ford.c
    algoritmos/kruskal.c
    algoritmos/dfs_bfs.c
    algoritmos/cycle_detection.c
)

set(DATA_STRUCTURE_SOURCES
    estructura_datos/union_find.c
    estructura_datos/priority_queue.c
    estructura_datos/hash_map.c
)

set(UTIL_SOURCES
    utils/file_io.c
    utils/visualization.c
    utils/benchmarks.c
        utils/file_io.h
        utils/visualization.h
)

set(TEST_SOURCES
    tests/test_dijkstra.c
    tests/test_bellman_ford.c
    tests/test_kruskal.c
    tests/test_traversals.c
    tests/test_cycles.c
)

# Biblioteca común
add_library(graph_algorithms STATIC
    ${GRAPH_SOURCES}
    ${ALGORITHM_SOURCES}
    ${DATA_STRUCTURE_SOURCES}
    ${UTIL_SOURCES}
)

# Enlazar matemáticas a la biblioteca
target_link_libraries(graph_algorithms m)

# Ejecutable principal
add_executable(main_program
    main.c
    ${TEST_SOURCES}
)
target_link_libraries(main_program graph_algorithms m)

# Ejecutable GPS
add_executable(gps_navigator
    navegacion_gps/main_gps.c
    navegacion_gps/gps_system.c
)
target_link_libraries(gps_navigator graph_algorithms m)

# Opciones de compilación
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(main_program PRIVATE -g -Wall -Wextra)
    target_compile_options(gps_navigator PRIVATE -g -Wall -Wextra)
endif()

message(STATUS "Configuración completa:")
message(STATUS "  - Biblioteca: libgraph_algorithms.a")
message(STATUS "  - Ejecutable principal: main_program")
message(STATUS "  - Ejecutable GPS: gps_navigator")
message(STATUS "  - Biblioteca matemática: enlazada")
