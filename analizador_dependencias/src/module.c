//
// Created by administrador on 6/9/25.
//

#include "../include/module.h"
#include <stdio.h>
#include <string.h>

void init_module(Module* module, const char* name, int id) {
    if (!module || !name) return;

    strncpy(module->name, name, MAX_NAME_LENGTH - 1);
    module->name[MAX_NAME_LENGTH - 1] = '\0';
    module->dep_count = 0;
    module->id = id;

    // Limpiar dependencias
    for (int i = 0; i < MAX_DEPENDENCIES; i++) {
        module->dependencies[i][0] = '\0';
    }
}

bool add_dependency_to_module(Module* module, const char* dependency) {
    if (!module || !dependency || module->dep_count >= MAX_DEPENDENCIES) {
        return false;
    }

    // Verificar si la dependencia ya existe
    if (module_depends_on(module, dependency)) {
        return true; // Ya existe, no es error
    }

    strncpy(module->dependencies[module->dep_count], dependency, MAX_NAME_LENGTH - 1);
    module->dependencies[module->dep_count][MAX_NAME_LENGTH - 1] = '\0';
    module->dep_count++;

    return true;
}

bool module_depends_on(const Module* module, const char* dependency) {
    if (!module || !dependency) return false;

    for (int i = 0; i < module->dep_count; i++) {
        if (strcmp(module->dependencies[i], dependency) == 0) {
            return true;
        }
    }
    return false;
}

void print_module(const Module* module) {
    if (!module) return;

    printf("Módulo: %s (ID: %d)\n", module->name, module->id);
    if (module->dep_count > 0) {
        printf("  Dependencias: ");
        for (int i = 0; i < module->dep_count; i++) {
            printf("%s", module->dependencies[i]);
            if (i < module->dep_count - 1) printf(", ");
        }
        printf("\n");
    } else {
        printf("  Sin dependencias\n");
    }
}

void clear_module_dependencies(Module* module) {
    if (!module) return;

    module->dep_count = 0;
    for (int i = 0; i < MAX_DEPENDENCIES; i++) {
        module->dependencies[i][0] = '\0';
    }
}