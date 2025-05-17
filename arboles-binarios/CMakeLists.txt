cmake_minimum_required(VERSION 3.31)
project(arboles_binarios C)

set(CMAKE_C_STANDARD 99)

add_executable(arboles_binarios nodo/ArbolImplementacion.c
        nodo/nodo.c
        nodo/nodo.h
        arbol-binario-BD/arbol.c
        arbol-binario-BD/arbol.h
        arbol-binario-BD/main.c)
