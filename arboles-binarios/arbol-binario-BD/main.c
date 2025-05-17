//
// Created by Lautaro on 13/05/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include "arbol.h"


int main() {
    // Crear una "tabla" simulada con algunos registros
    RegistroE tabla[10];

    // Inicializar registros
    tabla[0] = (RegistroE){1001, "Juan Pérez", 5000.0};
    tabla[1] = (RegistroE){1500, "Ana García", 7500.0};
    tabla[2] = (RegistroE){1250, "Carlos López", 3200.0};
    tabla[3] = (RegistroE){1800, "María Rodríguez", 9100.0};
    tabla[4] = (RegistroE){1100, "Pedro Sánchez", 4300.0};
    // ... más registros

    // Crear un índice (árbol B) para la tabla
    BTree* indice = inicializar();

    // Construir el índice insertando claves y punteros a registros
    for (int i = 0; i < 5; i++) {
        insertar(indice, tabla[i].id, &tabla[i]);
    }

    // Mostrar la estructura del índice
    printf("Índice después de insertar registros:\n");
    imprimirArbol(indice);

    // Simular una operación SELECT con WHERE
    printf("\nRealizando SELECT * FROM tabla WHERE id = 1500\n");
    int id_buscado = 1500;
    RegistroE* resultado = (RegistroE*)buscarEnArbol(indice, id_buscado);

    if (resultado) {
        printf("Registro encontrado:\n");
        printf("ID: %d, Nombre: %s, Saldo: %.2f\n",
               resultado->id, resultado->nombre, resultado->saldo);
    } else {
        printf("Registro con ID %d no encontrado\n", id_buscado);
    }

    // Simular una operación UPDATE
    printf("\nRealizando UPDATE tabla SET saldo = 8000 WHERE id = 1500\n");
    resultado = (RegistroE*)buscarEnArbol(indice, 1500);
    if (resultado) {
        resultado->saldo = 8000.0;
        printf("Registro actualizado. Nuevo saldo: %.2f\n", resultado->saldo);
    }

    // Liberar memoria
    liberarArbol(indice);

    return 0;
}

