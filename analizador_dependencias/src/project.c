//
// Created by administrador on 6/9/25.
//

#include "../include/project.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void init_project(CodeProject* project, const char* name) {
    if (!project) return;

    project->module_count = 0;
    project->dependency_graph = NULL;

    if (name) {
        strncpy(project->project_name, name, 99);
        project->project_name[99] = '\0';
    } else {
        strcpy(project->project_name, "Unnamed Project");
    }

    // Limpiar módulos
    for (int i = 0; i < MAX_MODULES; i++) {
        project->modules[i].name[0] = '\0';
        project->modules[i].dep_count = 0;
        project->modules[i].id = -1;
    }
}

void destroy_project(CodeProject* project) {
    if (!project) return;

    if (project->dependency_graph) {
        destroy_graph(project->dependency_graph);
        project->dependency_graph = NULL;
    }
}

int find_module_id(const CodeProject* project, const char* name) {
    if (!project || !name) return -1;

    for (int i = 0; i < project->module_count; i++) {
        if (strcmp(project->modules[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

bool add_module_to_project(CodeProject* project, const char* name) {
    if (!project || !name || project->module_count >= MAX_MODULES) {
        return false;
    }

    // Verificar si ya existe
    if (find_module_id(project, name) != -1) {
        return true; // Ya existe
    }

    init_module(&project->modules[project->module_count], name, project->module_count);
    project->module_count++;

    return true;
}

bool add_dependency_to_project(CodeProject* project, const char* module, const char* dependency) {
    if (!project || !module || !dependency) return false;

    // Asegurar que ambos módulos existan
    if (!add_module_to_project(project, module) ||
        !add_module_to_project(project, dependency)) {
        return false;
    }

    int module_id = find_module_id(project, module);
    if (module_id == -1) return false;

    return add_dependency_to_module(&project->modules[module_id], dependency);
}

void build_dependency_graph(CodeProject* project) {
    if (!project) return;

    // Destruir grafo anterior si existe
    if (project->dependency_graph) {
        destroy_graph(project->dependency_graph);
    }

    project->dependency_graph = create_graph(project->module_count);
    if (!project->dependency_graph) return;

    // Construir aristas basadas en dependencias de módulos
    for (int i = 0; i < project->module_count; i++) {
        Module* mod = &project->modules[i];
        for (int j = 0; j < mod->dep_count; j++) {
            int dep_id = find_module_id(project, mod->dependencies[j]);
            if (dep_id != -1) {
                add_edge(project->dependency_graph, i, dep_id);
            }
        }
    }
}

void print_project_summary(const CodeProject* project) {
    if (!project) return;

    printf("\n📊 RESUMEN DEL PROYECTO: %s\n", project->project_name);
    printf("================================\n");
    printf("Módulos totales: %d\n", project->module_count);

    int total_dependencies = 0;
    for (int i = 0; i < project->module_count; i++) {
        total_dependencies += project->modules[i].dep_count;
    }
    printf("Dependencias totales: %d\n", total_dependencies);

    printf("\nDetalle de módulos:\n");
    for (int i = 0; i < project->module_count; i++) {
        printf("• ");
        print_module(&project->modules[i]);
    }
}

bool load_project_from_file(CodeProject* project, const char* filename) {
    if (!project || !filename) return false;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return false;
    }

    char line[256];
    char module[MAX_NAME_LENGTH];
    char dependency[MAX_NAME_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        // Formato: "module:dependency1,dependency2,..."
        char* colon = strchr(line, ':');
        if (!colon) continue;

        *colon = '\0';
        strcpy(module, line);

        // Parsear dependencias
        char* deps = colon + 1;
        char* token = strtok(deps, ",\n");

        while (token) {
            // Remover espacios en blanco
            while (*token == ' ') token++;
            strcpy(dependency, token);

            add_dependency_to_project(project, module, dependency);
            token = strtok(NULL, ",\n");
        }
    }

    fclose(file);
    return true;
}

void save_project_to_file(const CodeProject* project, const char* filename) {
    if (!project || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: No se pudo crear el archivo %s\n", filename);
        return;
    }

    fprintf(file, "# Proyecto: %s\n", project->project_name);
    fprintf(file, "# Formato: modulo:dependencia1,dependencia2,...\n\n");

    for (int i = 0; i < project->module_count; i++) {
        const Module* mod = &project->modules[i];
        fprintf(file, "%s:", mod->name);

        for (int j = 0; j < mod->dep_count; j++) {
            fprintf(file, "%s", mod->dependencies[j]);
            if (j < mod->dep_count - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Proyecto guardado en %s\n", filename);
}