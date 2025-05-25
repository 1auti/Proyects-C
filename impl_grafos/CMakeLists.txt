cmake_minimum_required(VERSION 3.28)
project(impl_grafos C)

set(CMAKE_C_STANDARD 99)

add_executable(impl_grafos matriz_adyacencia.c
        lista_adyacencia.c)
