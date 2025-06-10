//
// Created by administrador on 6/9/25.
//

#ifndef MODULE_H
#define MODULE_H


#define MAX_NAME_LENGTH 50
#define MAX_DEPENDENCIES 20
#include <stdbool.h>

typedef struct {
    char name[MAX_NAME_LENGTH];
    char dependencies[MAX_DEPENDENCIES][MAX_NAME_LENGTH];
    int dep_count;
    int id;
} Module;

// Funciones para gestión de módulos
void init_module(Module* module, const char* name, int id);
bool add_dependency_to_module(Module* module, const char* dependency);
void print_module(const Module* module);
bool module_depends_on(const Module* module, const char* dependency);
void clear_module_dependencies(Module* module);

#endif //MODULE_H
