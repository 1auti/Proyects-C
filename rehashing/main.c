#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}

// Verificar si necesita rehashing
int needs_rehash(HashTable* table) {
    double load_factor = (double)table->count / table->size;
    return load_factor > 0.75;
}

// Rehashing para encadenamiento
void rehash_chaining(HashTable* table) {
    int old_size = table->size;
    Node** old_buckets = table->buckets;

    // Crear nueva tabla con tamaño doble
    table->size = old_size * 2;
    table->buckets = calloc(table->size, sizeof(Node*));
    table->count = 0;

    // Reinsertar todos los elementos
    for (int i = 0; i < old_size; i++) {
        Node* current = old_buckets[i];
        while (current) {
            Node* temp = current;
            current = current->next;

            // Reinsertar nodo
            unsigned int new_index = hash_function(temp->key, table->size);
            temp->next = table->buckets[new_index];
            table->buckets[new_index] = temp;
            table->count++;
        }
    }

    free(old_buckets);
}

// Rehashing para direccionamiento abierto
void rehash_open_addressing(HashTableOpen* table) {
    int old_size = table->size;
    Entry* old_entries = table->entries;

    // Crear nueva tabla
    table->size = old_size * 2;
    table->entries = calloc(table->size, sizeof(Entry));
    int old_count = table->count;
    table->count = 0;
    table->deleted_count = 0;

    // Reinsertar elementos válidos
    for (int i = 0; i < old_size; i++) {
        if (old_entries[i].is_occupied && !old_entries[i].is_deleted) {
            insert_linear(table, old_entries[i].key, old_entries[i].value);
            free(old_entries[i].key);
        } else if (old_entries[i].is_deleted) {
            free(old_entries[i].key);
        }
    }

    free(old_entries);
}

// Función de inserción con rehashing automático
void insert_with_rehash(HashTable* table, const char* key, int value) {
    if (needs_rehash(table)) {
        printf("Rehashing tabla de %d a %d\n", table->size, table->size * 2);
        rehash_chaining(table);
    }
    insert(table, key, value);
}