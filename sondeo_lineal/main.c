#include <stdio.h>
#include <stdlib.h>
#include "string.h"

/**
Este código implementa una tabla hash con direccionamiento abierto usando sondeo lineal y eliminación perezosa (lazy deletion),
que es una técnica para "marcar" entradas como eliminadas sin quitarlas físicamente de la tabla.
 */

typedef struct Entry {
    char * key; // Clave
    int value; // Valor asociado
    int is_occupied; // Si esta ocupado
    int is_deleted; // para una eliminacion lazy
}Entry;

typedef struct HashTableOpen {
    Entry * entries; // array de entradas
    int size;
    int count;
    int deleted_count;
}HashTableOpen;

int main(void) {
    printf("Hello, World!\n");
    return 0;
}

HashTableOpen * created_table_open(int size) {
    HashTableOpen * table = malloc(sizeof(HashTableOpen));
    table->count = 0;
    table->size = size;
    table->entries = calloc(size,sizeof(Entry)); //Usa calloc para inicializar todo el array entries con ceros (muy útil porque is_occupied y is_deleted empiezan en 0).
    table->deleted_count = 0;
    return table;
}

// INSERTAR CON SONDEO LINEAL
void insert_linear(HashTableOpen * table_open, const char * key, int value) {
    if(table_open->count >= table_open->size * 0.75) {
        // La table supero la capacidad del 75%
        printf("La tabla esta llena");
        return;
    }

    // utilizamos el algoritmo de Daniel
    unsigned int index = hash_fuction(key,table_open->size); // calcula el indece

    //  Realiza el Sondeo lineal , si la celda esta ocupada y no esta eliminada , prueba la siguiente
    while(table_open->entries[index].is_occupied && !table_open->entries[index].is_deleted) {
        // si encuentra una clave igual actualiza el valor o una posicoion eliminada la reutiliza
        if(strcmp(table_open->entries[index].key,key)==0) {
            table_open->entries[index].value = value; // Actualizar
            return;
        }
        index = (index +  1) % table_open->size;

    }

    // Insertar en posicion encontrada
    if(table_open->entries[index].is_deleted) {
        free(table_open->entries[index].key);
        table_open->deleted_count--;
    }

    // Asigna clave y valor, y marca la celda como ocupada y no eliminada.
    table_open->entries[index].is_deleted=0;
    table_open->entries[index].value = value;
    table_open->entries[index].is_occupied = 1;
    table_open->entries[index].key= strdup(key);
    table_open->count++;
}

// Buscar con sondeo lineal
int * search_linear(HashTableOpen * table_open,const char * key) {
    // la posicion donde se puede almacenar
    unsigned int index = hash_fuction(key,table_open->size);
    int start_index = index;

    do {
        if(!table_open->entries[index].is_occupied) {
            return NULL; // Posicion vacia, elemento no existe
        }

        if(!table_open->entries[index].is_deleted &&
            strcmp(table_open->entries[index].key,key)== 0) {
            return &table_open->entries[index].value;
        }
        index = (index +  1) % table_open->size;
    }while (index != start_index);

    return NULL; // NO encontrado
}

// Eliminar (lazy deletion)
int delete_linear(HashTableOpen* table, const char* key) {
    unsigned int index = hash_function(key, table->size);
    int start_index = index;

    do {
        if (!table->entries[index].is_occupied) {
            return 0; // No encontrado
        }

        if (!table->entries[index].is_deleted &&
            strcmp(table->entries[index].key, key) == 0) {
            table->entries[index].is_deleted = 1;
            table->deleted_count++;
            table->count--;
            return 1; // Éxito
            }

        index = (index + 1) % table->size;
    } while (index != start_index);

    return 0; // No encontrado
}