/**
 * Implementación corregida de HAT-Trie
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26     // Tamaño del alfabeto (a-z)
#define BURST_THRESHOLD 8    // Umbral para convertir un nodo a contenedor
#define INITIAL_HASH_SIZE 16 // Tamaño inicial de la tabla hash

/*
 * Tipos de nodos en HAT-Trie
 */
typedef enum {
    TRIE_NODE,      // Nodo tradicional de trie
    CONTAINER_NODE  // Nodo contenedor (usa tabla hash)
} NodeType;

/*
 * Entrada en la tabla hash
 */
typedef struct HashEntry {
    char* suffix;          // Sufijo almacenado
    struct HashEntry* next; // Siguiente entrada (manejo de colisiones)
} HashEntry;

/*
 * Tabla hash para los nodos contenedores
 */
typedef struct {
    HashEntry** buckets;   // Array de cubetas
    int size;              // Tamaño de la tabla
    int count;             // Número de entradas
    char prefix[32];       // Prefijo común para todas las entradas
    int prefixLen;         // Longitud del prefijo
} HashTable;

/*
 * Nodo de HAT-Trie (puede ser nodo trie o nodo contenedor)
 */
typedef struct HATNode {
    NodeType type;         // Tipo de nodo

    union {
        // Para nodos trie
        struct {
            struct HATNode* children[ALPHABET_SIZE]; // Hijos (uno por carácter)
            bool isEnd[ALPHABET_SIZE];               // Marca fin de palabra
            int wordCount;                           // Contador de palabras
        };

        // Para nodos contenedor
        struct {
            HashTable* container;  // Tabla hash para almacenar sufijos
            char prefix;           // Carácter prefijo del contenedor
        };
    };
} HATNode;

// Declaraciones adelantadas
void freeHATNode(HATNode* node);
void hashInsert(HashTable* table, const char* suffix);
bool hashSearch(HashTable* table, const char* suffix);

/*
 * Funciones para TablaHash
 */

// Crear una nueva tabla hash
HashTable* createHashTable(const char* prefix, int prefixLen) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) return NULL;

    table->size = INITIAL_HASH_SIZE;
    table->count = 0;
    table->buckets = (HashEntry**)calloc(table->size, sizeof(HashEntry*));

    if (!table->buckets) {
        free(table);
        return NULL;
    }

    if (prefix && prefixLen > 0) {
        strncpy(table->prefix, prefix, prefixLen < 31 ? prefixLen : 31);
        table->prefix[prefixLen < 31 ? prefixLen : 31] = '\0';
        table->prefixLen = prefixLen;
    } else {
        table->prefix[0] = '\0';
        table->prefixLen = 0;
    }

    return table;
}

// Función hash sencilla
unsigned int hashFunction(const char* key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + (*key++);
    }
    return hash;
}

// Insertar en la tabla hash
void hashInsert(HashTable* table, const char* suffix) {
    // Si es NULL o cadena vacía, salir
    if (!table || !suffix) return;

    // Calcular índice de hash
    unsigned int hash = hashFunction(suffix) % table->size;

    // Verificar si ya existe
    HashEntry* current = table->buckets[hash];
    while (current) {
        if (strcmp(current->suffix, suffix) == 0)
            return; // Ya existe, no hacer nada
        current = current->next;
    }

    // Crear nueva entrada
    HashEntry* newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    if (!newEntry) return;

    newEntry->suffix = strdup(suffix);
    newEntry->next = table->buckets[hash];
    table->buckets[hash] = newEntry;
    table->count++;
}

// Buscar en la tabla hash
bool hashSearch(HashTable* table, const char* suffix) {
    if (!table || !suffix) return false;

    unsigned int hash = hashFunction(suffix) % table->size;
    HashEntry* current = table->buckets[hash];

    while (current) {
        if (strcmp(current->suffix, suffix) == 0)
            return true;
        current = current->next;
    }

    return false;
}

// Liberar la tabla hash
void freeHashTable(HashTable* table) {
    if (!table) return;

    for (int i = 0; i < table->size; i++) {
        HashEntry* current = table->buckets[i];
        while (current) {
            HashEntry* temp = current;
            current = current->next;
            free(temp->suffix);
            free(temp);
        }
    }

    free(table->buckets);
    free(table);
}

/*
 * Funciones para HAT-Trie
 */

// Crear un nuevo nodo trie
HATNode* createTrieNode() {
    HATNode* node = (HATNode*)malloc(sizeof(HATNode));
    if (!node) return NULL;

    node->type = TRIE_NODE;
    node->wordCount = 0;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
        node->isEnd[i] = false;
    }

    return node;
}

// Crear un nuevo nodo contenedor
HATNode* createContainerNode(char prefix, const char* prefixStr, int prefixLen) {
    HATNode* node = (HATNode*)malloc(sizeof(HATNode));
    if (!node) return NULL;

    node->type = CONTAINER_NODE;
    node->prefix = prefix;
    node->container = createHashTable(prefixStr, prefixLen);

    if (!node->container) {
        free(node);
        return NULL;
    }

    return node;
}

// Función recursiva para recolectar palabras
void collectWords(HATNode* current, int depth, char* buffer,
                 int prefixLen, HashTable* temp) {
    if (!current) return;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (current->isEnd[i] || current->children[i]) {
            buffer[prefixLen + depth] = 'a' + i;

            if (current->isEnd[i]) {
                // Es fin de palabra, guardar en la tabla hash
                buffer[prefixLen + depth + 1] = '\0';
                hashInsert(temp, buffer + prefixLen);
            }

            if (current->children[i]) {
                // Seguir recolectando recursivamente
                collectWords(current->children[i], depth + 1, buffer, prefixLen, temp);
            }
        }
    }
}

// Liberar nodo HAT-Trie (recursivamente)
void freeHATNode(HATNode* node) {
    if (!node) return;

    if (node->type == TRIE_NODE) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i]) {
                freeHATNode(node->children[i]);
            }
        }
    } else {
        // Es un nodo contenedor
        freeHashTable(node->container);
    }

    free(node);
}

// Convertir nodo trie a nodo contenedor
void convertToContainer(HATNode* node, char* prefixStr, int prefixLen) {
    if (!node || node->type != TRIE_NODE) return;

    // Crear tabla hash temporal
    HashTable* temp = createHashTable(prefixStr, prefixLen);
    if (!temp) return;

    // Recolectar todas las palabras del nodo y sus hijos
    char buffer[256]; // Buffer para construir palabras
    if (prefixLen > 0) {
        strncpy(buffer, prefixStr, prefixLen);
    }

    // Iniciar recolección
    collectWords(node, 0, buffer, prefixLen, temp);

    // Liberar todos los nodos hijos
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            freeHATNode(node->children[i]);
            node->children[i] = NULL;
        }
    }

    // Convertir el nodo a tipo contenedor
    node->type = CONTAINER_NODE;
    node->container = temp;
}

// Insertar palabra en HAT-Trie - CORREGIDA
void insert(HATNode* root, const char* key) {
    if (!root || !key) return;

    HATNode* current = root;
    const char* start = key;
    const char* keyPtr = key;

    while (*keyPtr) {
        if (current->type == TRIE_NODE) {
            int index = *keyPtr - 'a';

            if (index < 0 || index >= ALPHABET_SIZE) {
                return; // Carácter fuera de rango
            }

            // Si el nodo tiene demasiadas palabras, considerar conversión
            if (current->wordCount > BURST_THRESHOLD) {
                int prefixLen = keyPtr - start;
                char prefix[256];

                if (prefixLen > 0) {
                    strncpy(prefix, start, prefixLen);
                    prefix[prefixLen] = '\0';
                } else {
                    prefix[0] = '\0';
                }

                convertToContainer(current, prefix, prefixLen);
                hashInsert(current->container, keyPtr);
                return;
            }

            // Normal trie insertion
            if (!current->children[index]) {
                current->children[index] = createTrieNode();
            }

            // CORRECCIÓN: Si es el final de la palabra
            if (*(keyPtr+1) == '\0') {
                current->isEnd[index] = true;
            }

            current = current->children[index];
            current->wordCount++;
            keyPtr++;
        }
        else { // CONTAINER_NODE
            // Insertar resto de la palabra en la tabla hash
            hashInsert(current->container, keyPtr);
            return;
        }
    }
}

// Buscar palabra en HAT-Trie - CORREGIDA
bool search(HATNode* root, const char* key) {
    if (!root || !key) return false;

    HATNode* current = root;
    const char* keyPtr = key;

    while (*keyPtr) {
        if (current->type == TRIE_NODE) {
            int index = *keyPtr - 'a';

            if (index < 0 || index >= ALPHABET_SIZE || !current->children[index])
                return false;

            // CORRECCIÓN: Verificar si es fin de palabra en el nodo actual
            if (*(keyPtr+1) == '\0' && current->isEnd[index])
                return true;

            current = current->children[index];
            keyPtr++;
        }
        else { // CONTAINER_NODE
            return hashSearch(current->container, keyPtr);
        }
    }

    return false;
}

// Función auxiliar para encontrar palabras desde un nodo - CORREGIDA
void findWordsFromNode(HATNode* node, char* buffer, int depth, int bufferLen) {
    if (!node) return;

    if (node->type == TRIE_NODE) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            buffer[bufferLen + depth] = 'a' + i;

            // CORRECCIÓN: Evitar duplicar el último carácter
            if (node->isEnd[i]) {
                buffer[bufferLen + depth + 1] = '\0';
                printf("%s\n", buffer);
            }

            if (node->children[i]) {
                findWordsFromNode(node->children[i], buffer, depth + 1, bufferLen);
            }
        }
    }
    else { // CONTAINER_NODE
        printf("(Palabras en contenedor omitidas)\n");
    }
}

// Función de autocompletado para HAT-Trie
void autocomplete(HATNode* root, const char* prefix) {
    if (!root || !prefix) return;

    HATNode* current = root;
    const char* originalPrefix = prefix;

    // Navegar hasta el nodo que representa el prefijo
    while (*prefix && current && current->type == TRIE_NODE) {
        int index = *prefix - 'a';

        if (index < 0 || index >= ALPHABET_SIZE || !current->children[index])
            return; // No hay palabras con este prefijo

        current = current->children[index];
        prefix++;
    }

    // Si llegamos a un nodo contenedor antes de completar el prefijo
    if (current && current->type == CONTAINER_NODE) {
        printf("Prefijo en contenedor (resultados omitidos)\n");
        return;
    }

    // Si hemos llegado al final del prefijo, buscar todas las palabras desde aquí
    char buffer[256];
    strcpy(buffer, originalPrefix);
    int len = strlen(buffer);

    // Buscar palabras
    findWordsFromNode(current, buffer, 0, len);
}

int main() {
    HATNode* hat = createTrieNode();

    // Insertar palabras
    printf("Insertando palabras en el HAT-Trie...\n");
    insert(hat, "cat");
    insert(hat, "car");
    insert(hat, "cart");
    insert(hat, "dog");
    insert(hat, "done");
    insert(hat, "dare");
    insert(hat, "apple");
    insert(hat, "april");
    insert(hat, "art");

    // Búsqueda de palabras
    printf("\nBuscando palabras en HAT-Trie:\n");
    printf("'cat': %s\n", search(hat, "cat") ? "Encontrado" : "No encontrado");
    printf("'car': %s\n", search(hat, "car") ? "Encontrado" : "No encontrado");
    printf("'cab': %s\n", search(hat, "cab") ? "Encontrado" : "No encontrado");
    printf("'done': %s\n", search(hat, "done") ? "Encontrado" : "No encontrado");

    // Probar autocompletado
    printf("\nAutocompletado para 'ca':\n");
    autocomplete(hat, "ca");

    printf("\nAutocompletado para 'a':\n");
    autocomplete(hat, "a");

    // Liberar memoria
    freeHATNode(hat);

    return 0;
}