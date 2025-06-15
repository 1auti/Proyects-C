#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}

// Segunda función hash
unsigned int hash2(const char* key, int table_size) {
    unsigned int hash = 0;
    /* Lo que hacemos aca es recorrer la clave -> 'abc' . Primero para a -> b -> c   */
    while (*key) {
        /*  Calculamos el valor ASCII de forma acumulativa  con a -> b -> c
         *  hash = 0 * 31 + 'a' = 0 + 97 = 97 (porque 'a' tiene valor ASCII 97)  -> hash = 97 * 31 + 'b' = 3007 + 98 = 3105 (porque 'b' tiene valor ASCII 98) -> hash = 3105 * 31 + 'c' = 96255 + 99 = 96354 (porque 'c' tiene valor ASCII 99)
         *  El valor final de hash será 96354, que es un número único para la clave "abc".
         */
        hash = hash * 31 + *key++; // porque 31 es un número primo que tiene algunas propiedades que lo hacen útil para la función hash:
    }
    return 7 - (hash % 7); // Número primo menor que table_size  para evitar conflictos futuros
}

void insert_double_hash(HashTableOpen* table, const char* key, int value) {
    // verificamos que la tabla no este llena
    if (table->count >= table->size * 0.75) {
        printf("Tabla llena\n");
        return;
    }

    // calculamos el indice de la key
    unsigned int index = hash_function(key, table->size);
    // calculamos el indice si da colision
    unsigned int step = hash2(key, table->size);
    int i = 0;

    // Mientras la posicion no este ocupada  y elimianda
    while (table->entries[index].is_occupied && !table->entries[index].is_deleted) {
        if (strcmp(table->entries[index].key, key) ==  0) {
            table->entries[index].value = value; // si laclave  ya exsite en la tabla simplemente actializamos su valor
            return;
        }

        // Se salta de lugar en lugar usando step hasta encontrar un hueco (estilo doble hashing).
        //Si se recorrió toda la tabla sin encontrar un hueco, aborta la inserción.

        index = (index + step) % table->size;
        i++;

        if (i >= table->size) {
            printf("No se pudo insertar\n");
            return;
        }
    }

    // Insertar elemento
    //Si la posición fue borrada anteriormente, se limpia su clave anterior y se actualiza el conteo de eliminados.
    if (table->entries[index].is_deleted) {
        free(table->entries[index].key);
        table->deleted_count--;
    }

    table->entries[index].key = strdup(key);
    table->entries[index].value = value;
    table->entries[index].is_occupied = 1;
    table->entries[index].is_deleted = 0;
    table->count++;
}