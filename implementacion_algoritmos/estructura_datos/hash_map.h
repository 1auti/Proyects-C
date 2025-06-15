#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASH_MAP_INITIAL_SIZE 16
#define HASH_MAP_LOAD_FACTOR 0.75

// Entrada del hash map
typedef struct HashEntry {
    char* key;
    void* value;
    struct HashEntry* next;
    bool isDeleted;
} HashEntry;

// Estructura principal del hash map
typedef struct {
    HashEntry** buckets;
    int size;
    int capacity;
    int numElements;
} HashMap;

// Prototipos de funciones principales
HashMap* createHashMap(int initialCapacity);
void destroyHashMap(HashMap* map);

// Operaciones básicas
bool hashMapPut(HashMap* map, const char* key, void* value);
void* hashMapGet(HashMap* map, const char* key);
bool hashMapRemove(HashMap* map, const char* key);
bool hashMapContains(HashMap* map, const char* key);

// Utilidades
int hashMapSize(HashMap* map);
bool hashMapIsEmpty(HashMap* map);
void hashMapClear(HashMap* map);
char** hashMapGetKeys(HashMap* map, int* numKeys);
void printHashMap(HashMap* map);

// Funciones auxiliares
unsigned int hashFunction(const char* key, int capacity);
void resizeHashMap(HashMap* map);
HashEntry* createHashEntry(const char* key, void* value);
void freeHashEntry(HashEntry* entry);

#endif