#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}

void insert_quadratic(HashTableOpen* table, const char* key, int value) {
    if (table->count >= table->size * 0.5) {
        printf("Factor de carga alto para sondeo cuadrático\n");
        return;
    }

    unsigned int index = hash_function(key, table->size);
    int i = 0;

    while (table->entries[index].is_occupied && !table->entries[index].is_deleted) {
        if (strcmp(table->entries[index].key, key) == 0) {
            table->entries[index].value = value;
            return;
        }

        i++;
        index = (hash_function(key, table->size) + i * i) % table->size;

        if (i >= table->size) {
            printf("No se pudo insertar, tabla llena\n");
            return;
        }
    }

    // Insertar elemento
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