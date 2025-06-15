cmake_minimum_required(VERSION 3.28)
project(algoritmo_kahn C)

set(CMAKE_C_STANDARD 11)

add_executable(algoritmo_kahn main.c
        queue/queue.h
        graph/graph.h
        graph/graph.c
        queue/queue.c)
