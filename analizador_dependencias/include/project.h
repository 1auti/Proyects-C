//
// Created by administrador on 6/9/25.
//

#ifndef PROJECT_H
#define PROJECT_H

#include "graph.h"
#include "module.h"

#define MAX_MODULES 100

typedef struct {
    Module modules[MAX_MODULES];
    int module_count;
    Graph* dependency_graph;
    char project_name[100];
} CodeProject;

// Funciones de gestión del proyecto
void init_project(CodeProject* project, const char* name);
void destroy_project(CodeProject* project);
bool add_module_to_project(CodeProject* project, const char* name);
bool add_dependency_to_project(CodeProject* project, const char* module, const char* dependency);
int find_module_id(const CodeProject* project, const char* name);
void build_dependency_graph(CodeProject* project);
void print_project_summary(const CodeProject* project);
bool load_project_from_file(CodeProject* project, const char* filename);
void save_project_to_file(const CodeProject* project, const char* filename);

#endif //PROJECT_H
