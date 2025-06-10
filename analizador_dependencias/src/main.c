#include "../include/project.h"
#include "../include/tarjan.h"
#include "../include/topological.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_header() {
    printf("🚀 ANALIZADOR DE DEPENDENCIAS DE CÓDIGO\n");
    printf("=======================================\n");
    printf("Versión 1.0 - Análisis profesional de dependencias\n\n");
}

void print_menu() {
    printf("\n📋 OPCIONES DISPONIBLES:\n");
    printf("========================\n");
    printf("1. Cargar proyecto de ejemplo\n");
    printf("2. Cargar proyecto desde archivo\n");
    printf("3. Agregar módulo manualmente\n");
    printf("4. Agregar dependencia\n");
    printf("5. Mostrar resumen del proyecto\n");
    printf("6. Analizar dependencias circulares\n");
    printf("7. Generar orden de compilación\n");
    printf("8. Generar script de build\n");
    printf("9. Guardar proyecto\n");
    printf("0. Salir\n");
    printf("\nSeleccione una opción: ");
}

void load_sample_project(CodeProject* project) {
    printf("🏗️  Cargando proyecto de ejemplo...\n");

    init_project(project, "SampleProject");

    // Agregar módulos y dependencias
    add_dependency_to_project(project, "main", "ui");
    add_dependency_to_project(project, "main", "database");
    add_dependency_to_project(project, "main", "config");

    add_dependency_to_project(project, "ui", "auth");
    add_dependency_to_project(project, "ui", "utils");

    add_dependency_to_project(project, "database", "auth");
    add_dependency_to_project(project, "database", "network");
    add_dependency_to_project(project, "database", "config");

    add_dependency_to_project(project, "auth", "utils");
    add_dependency_to_project(project, "auth", "network");

    add_dependency_to_project(project, "network", "config");

    // Dependencia circular opcional (descomentado para probar)
    // add_dependency_to_project(project, "auth", "ui");

    build_dependency_graph(project);
    printf("✅ Proyecto de ejemplo cargado exitosamente!\n");
}

void interactive_add_module(CodeProject* project) {
    char module_name[MAX_NAME_LENGTH];

    printf("Ingrese el nombre del módulo: ");
    if (scanf("%49s", module_name) == 1) {
        if (add_module_to_project(project, module_name)) {
            printf("✅ Módulo '%s' agregado exitosamente!\n", module_name);
        } else {
            printf("❌ Error al agregar el módulo.\n");
        }
    }
}

void interactive_add_dependency(CodeProject* project) {
    char module[MAX_NAME_LENGTH];
    char dependency[MAX_NAME_LENGTH];

    printf("Ingrese el nombre del módulo: ");
    if (scanf("%49s", module) != 1) return;

    printf("Ingrese la dependencia: ");
    if (scanf("%49s", dependency) != 1) return;

    if (add_dependency_to_project(project, module, dependency)) {
        printf("✅ Dependencia agregada: %s -> %s\n", module, dependency);

        // Reconstruir grafo
        build_dependency_graph(project);
    } else {
        printf("❌ Error al agregar la dependencia.\n");
    }
}

void analyze_and_generate_order(CodeProject* project) {
    if (!project->dependency_graph) {
        printf("❌ Primero debe cargar o crear un proyecto.\n");
        return;
    }

    // Analizar dependencias circulares
    analyze_circular_dependencies(project);

    // Generar orden de compilación
    TopologicalResult* result = create_topological_result(project->module_count);
    if (!result) {
        printf("❌ Error al crear estructura para ordenación topológica.\n");
        return;
    }

    printf("\n🔧 GENERANDO ORDEN DE COMPILACIÓN...\n");

    if (topological_sort_kahn(project->dependency_graph, result)) {
        print_compilation_order(result, project);

        // Ofrecer generar script
        char response;
        printf("¿Desea generar un script de build automático? (s/n): ");
        if (scanf(" %c", &response) == 1 && (response == 's' || response == 'S')) {
            char script_name[256];
            snprintf(script_name, sizeof(script_name), "build_%s.sh", project->project_name);

            if (generate_build_script(result, project, script_name)) {
                printf("📄 Script generado: %s\n", script_name);
            }
        }
    } else {
        printf("❌ No es posible generar orden de compilación debido a ciclos.\n");
    }

    destroy_topological_result(result);
}

void load_from_file(CodeProject* project) {
    char filename[256];

    printf("Ingrese el nombre del archivo: ");
    if (scanf("%255s", filename) == 1) {
        init_project(project, "LoadedProject");

        if (load_project_from_file(project, filename)) {
            build_dependency_graph(project);
            printf("✅ Proyecto cargado desde %s\n", filename);
        } else {
            printf("❌ Error al cargar el archivo.\n");
        }
    }
}

void save_to_file(CodeProject* project) {
    if (project->module_count == 0) {
        printf("❌ No hay proyecto para guardar.\n");
        return;
    }

    char filename[256];
    printf("Ingrese el nombre del archivo de salida: ");
    if (scanf("%255s", filename) == 1) {
        save_project_to_file(project, filename);
    }
}

int main() {
    CodeProject project;
    init_project(&project, "");

    print_header();

    int choice;
    do {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            printf("❌ Entrada inválida.\n");
            // Limpiar buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch (choice) {
            case 1:
                load_sample_project(&project);
                break;

            case 2:
                load_from_file(&project);
                break;

            case 3:
                interactive_add_module(&project);
                break;

            case 4:
                interactive_add_dependency(&project);
                break;

            case 5:
                print_project_summary(&project);
                if (project.dependency_graph) {
                    const char* module_names[MAX_MODULES];
                    for (int i = 0; i < project.module_count; i++) {
                        module_names[i] = project.modules[i].name;
                    }
                    print_graph(project.dependency_graph, module_names);
                }
                break;

            case 6:
                if (project.dependency_graph) {
                    analyze_circular_dependencies(&project);
                } else {
                    printf("❌ Primero debe cargar un proyecto.\n");
                }
                break;

            case 7:
                analyze_and_generate_order(&project);
                break;

            case 8:
                if (project.dependency_graph) {
                    TopologicalResult* result = create_topological_result(project.module_count);
                    if (result && topological_sort_kahn(project.dependency_graph, result)) {
                        char script_name[256];
                        snprintf(script_name, sizeof(script_name), "build_%s.sh", project.project_name);
                        generate_build_script(result, &project, script_name);
                    }
                    destroy_topological_result(result);
                } else {
                    printf("❌ Primero debe cargar un proyecto.\n");
                }
                break;

            case 9:
                save_to_file(&project);
                break;

            case 0:
                printf("👋 ¡Hasta luego!\n");
                break;

            default:
                printf("❌ Opción inválida. Intente de nuevo.\n");
        }

    } while (choice != 0);

    destroy_project(&project);

    return 0;
}