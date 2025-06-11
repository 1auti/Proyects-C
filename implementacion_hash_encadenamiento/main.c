#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char * key;
    int value;
    struct Node * next;
}Node;

typedef struct {
    Node ** buckets; // array de punteros a Node. Cada uno es una lista
    int size;
    int count;
}HashTable;

int main(void)
{
    printf("Hello, World!\n");
    return 0;
}

// funciones hash
unsigned int hash_function(const char * key, int table_size) {
    // funcion efectiva de Daniel
    unsigned int hash = 5381;
    int c;
    while((c = *key++)) {
        hash = ((hash << 5 ) + hash) + c;
    }
    return hash % table_size;
}

// Crear nueva tabla
HashTable * create_table(int size) {
    HashTable * table = malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0 ;
    table->buckets = calloc(size,sizeof(Node*));
    return table;
}


// Crear nodo
Node * create_node(const char * key, int value) {
    Node * node = malloc(sizeof(Node));
    node->key = strdup(key); // duplica el string
    node->value = value;
    node->next = NULL;
    return node;
}

// Insertar elemento
void insertar(HashTable * table, const char * key, int value) {
    unsigned int index = hash_function(key,table->size);
    Node * head = table->buckets[index];

    // BUscar si la clave ya existe
    Node * current = head;
    while (current) {
        if(strcmp(current->key,key) == 0) {
            current->value = value; // Actualizar valor
            return;
        }
        current = current->next;
    }

    // Insertar nuevo nodo al inicio
    Node * new_node = create_node(key,value);
    new_node->next = head;
    table->buckets[index] = new_node;
    table->count++;
}

// Buscar elemento
int * search(HashTable * table, const char * key) {
    unsigned int index = hash_function(key,table->size);
    Node * current = table->buckets[index];

    while (current) {
        if(strcmp(current->key,key) == 0) {
            return  &current->value;
        }
        current = current->next;
    }
    return NULL;
}


// Eliminar elemento
int delete(HashTable * table, const char * key){
    unsigned int index = hash_function(key,table->size);
    Node * current = table->buckets[index];
    Node * prev = NULL;

    while(current) {
        if(strcmp(current->key,key) == 0) {
            if(prev) {
                prev->next = current->next;
            } else {
                table->buckets[index] = current->next;
            }
            free(current->key);
            free(current);
            table->count--;
            return 1; // Éxito
        }
        prev = current;
        current = current->next;
    }
    return 0; // No encontrado
}

// Liberar memoria
void free_table(HashTable * table) {
    for(int i = 0 ; i < table->size; i++) {
        Node * current = table->buckets[i];
        while(current) {
            Node * temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}


