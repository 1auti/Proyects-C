cmake_minimum_required(VERSION 3.28)
project(analizador_dependencias C)

set(CMAKE_C_STANDARD 11)

add_executable(analizador_dependencias src/main.c
        include/graph.h
        include/module.h
        include/tarjan.h
        include/topological.h
        include/project.h
        src/graph.c
        src/module.c
        src/tarjan.c
        src/topological.c
        src/project.c)
