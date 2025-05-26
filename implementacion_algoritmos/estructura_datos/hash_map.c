#include "hash_map.h"

// Función hash simple pero efectiva
unsigned int hashFunction(const char* key, int capacity) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % capacity;
}

// Crear nueva entrada del hash map
HashEntry* createHashEntry(const char* key, void* value) {
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    if (!entry) return NULL;
    
    entry->key = (char*)malloc(strlen(key) + 1);
    strcpy(entry->key, key);
    entry->value = value;
    entry->next = NULL;
    entry->isDeleted = false;
    
    return entry;
}

// Liberar entrada del hash map
void freeHashEntry(HashEntry* entry) {
    if (entry) {
        free(entry->key);
        free(entry);
    }
}

// Crear hash map
HashMap* createHashMap(int initialCapacity) {
    if (initialCapacity <= 0) {
        initialCapacity = HASH_MAP_INITIAL_SIZE;
    }
    
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) return NULL;
    
    map->buckets = (HashEntry**)calloc(initialCapacity, sizeof(HashEntry*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    map->capacity = initialCapacity;
    map->size = 0;
    map->numElements = 0;
    
    return map;
}

// Redimensionar hash map cuando se alcanza el factor de carga
void resizeHashMap(HashMap* map) {
    int oldCapacity = map->capacity;
    HashEntry** oldBuckets = map->buckets;
    
    // Duplicar la capacidad
    map->capacity *= 2;
    map->buckets = (HashEntry**)calloc(map->capacity, sizeof(HashEntry*));
    if (!map->buckets) {
        map->capacity = oldCapacity;
        map->buckets = oldBuckets;
        return; // Falló el resize
    }
    
    int oldSize = map->size;
    map->size = 0;
    
    // Reinsertar todos los elementos
    for (int i = 0; i < oldCapacity; i++) {
        HashEntry* current = oldBuckets[i];
        while (current) {
            HashEntry* next = current->next;
            if (!current->isDeleted) {
                // Recalcular hash y reinsertar
                unsigned int newIndex = hashFunction(current->key, map->capacity);
                current->next = map->buckets[newIndex];
                map->buckets[newIndex] = current;
                map->size++;
            } else {
                freeHashEntry(current);
            }
            current = next;
        }
    }
    
    free(oldBuckets);
    printf("Hash map redimensionado: %d -> %d elementos\n", oldSize, map->size);
}

// Insertar o actualizar valor
bool hashMapPut(HashMap* map, const char* key, void* value) {
    if (!map || !key) return false;
    
    // Verificar si necesitamos redimensionar
    if ((double)map->numElements / map->capacity >= HASH_MAP_LOAD_FACTOR) {
        resizeHashMap(map);
    }
    
    unsigned int index = hashFunction(key, map->capacity);
    HashEntry* current = map->buckets[index];
    
    // Buscar si la clave ya existe
    while (current) {
        if (!current->isDeleted && strcmp(current->key, key) == 0) {
            // Actualizar valor existente
            current->value = value;
            return true;
        }
        current = current->next;
    }
    
    // Crear nueva entrada
    HashEntry* newEntry = createHashEntry(key, value);
    if (!newEntry) return false;
    
    // Insertar al inicio de la lista
    newEntry->next = map->buckets[index];
    map->buckets[index] = newEntry;
    map->size++;
    map->numElements++;
    
    return true;
}

// Obtener valor por clave
void* hashMapGet(HashMap* map, const char* key) {
    if (!map || !key) return NULL;
    
    unsigned int index = hashFunction(key, map->capacity);
    HashEntry* current = map->buckets[index];
    
    while (current) {
        if (!current->isDeleted && strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL; // No encontrado
}

// Verificar si contiene una clave
bool hashMapContains(HashMap* map, const char* key) {
    return hashMapGet(map, key) != NULL;
}

// Eliminar entrada por clave
bool hashMapRemove(HashMap* map, const char* key) {
    if (!map || !key) return false;
    
    unsigned int index = hashFunction(key, map->capacity);
    HashEntry* current = map->buckets[index];
    HashEntry* prev = NULL;
    
    while (current) {
        if (!current->isDeleted && strcmp(current->key, key) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                map->buckets[index] = current->next;
            }
            
            freeHashEntry(current);
            map->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false; // No encontrado
}

// Obtener tamaño
int hashMapSize(HashMap* map) {
    return map ? map->size : 0;
}

// Verificar si está vacío
bool hashMapIsEmpty(HashMap* map) {
    return hashMapSize(map) == 0;
}

// Limpiar todos los elementos
void hashMapClear(HashMap* map) {
    if (!map) return;
    
    for (int i = 0; i < map->capacity; i++) {
        HashEntry* current = map->buckets[i];
        while (current) {
            HashEntry* next = current->next;
            freeHashEntry(current);
            current = next;
        }
        map->buckets[i] = NULL;
    }
    
    map->size = 0;
    map->numElements = 0;
}

// Obtener todas las claves
char** hashMapGetKeys(HashMap* map, int* numKeys) {
    if (!map || !numKeys) return NULL;
    
    *numKeys = map->size;
    if (map->size == 0) return NULL;
    
    char** keys = (char**)malloc(map->size * sizeof(char*));
    int keyIndex = 0;
    
    for (int i = 0; i < map->capacity; i++) {
        HashEntry* current = map->buckets[i];
        while (current) {
            if (!current->isDeleted) {
                keys[keyIndex] = (char*)malloc(strlen(current->key) + 1);
                strcpy(keys[keyIndex], current->key);
                keyIndex++;
            }
            current = current->next;
        }
    }
    
    return keys;
}

// Imprimir hash map
void printHashMap(HashMap* map) {
    if (!map) {
        printf("Hash map es NULL\n");
        return;
    }
    
    printf("\n=== HASH MAP ===\n");
    printf("Capacidad: %d, Tamaño: %d, Elementos: %d\n", 
           map->capacity, map->size, map->numElements);
    printf("Factor de carga: %.2f\n", (double)map->numElements / map->capacity);
    
    for (int i = 0; i < map->capacity; i++) {
        if (map->buckets[i]) {
            printf("Bucket %d: ", i);
            HashEntry* current = map->buckets[i];
            while (current) {
                if (!current->isDeleted) {
                    printf("[%s] -> ", current->key);
                }
                current = current->next;
            }
            printf("NULL\n");
        }
    }
    printf("===============\n");
}

// Destruir hash map
void destroyHashMap(HashMap* map) {
    if (!map) return;
    
    hashMapClear(map);
    free(map->buckets);
    free(map);
}