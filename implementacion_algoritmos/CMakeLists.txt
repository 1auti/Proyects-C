cmake_minimum_required(VERSION 3.28)
project(implementacion_algoritmos C)

set(CMAKE_C_STANDARD 99)

add_executable(implementacion_algoritmos main.c
        graph/graph.h
        graph/graph.c
        graph/lista_adyacente.c
        graph/matrix_adyacente.c
        algoritmos/dijkstra.h
        algoritmos/dijkstra.c
        algoritmos/bellman_ford.c
        algoritmos/bellman_ford.h
        algoritmos/kruskal.c
        algoritmos/kruskal.h
        algoritmos/dfs_bfs.c
        algoritmos/dfs_bfs.h
        algoritmos/cycle_detection.c
        estructura_datos/union_find.c
        estructura_datos/union_find.h
        estructura_datos/priority_queue.h.c
        estructura_datos/priority_queue.h
        estructura_datos/hash_map.c
        estructura_datos/hash_map.h
        utils/file_io.c
        utils/visualization.c
        utils/benchmarks.c
        tests/test_dijkstra.c
        tests/test_bellman_ford.c
        tests/test_kruskal.c
        tests/test_traversals.c
        tests/test_cycles.c
        navegacion_gps/gps_system.c
        navegacion_gps/gps_system.h
        algoritmos/common_types.h
        algoritmos/cycle_detection.h
        navegacion_gps/main_gps.c)
