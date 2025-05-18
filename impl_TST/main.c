/**
 * Implementación completa de Tries y TST con funcionalidades avanzadas
 * Incluye:
 * - Trie estándar
 * - Ternary Search Trie (TST)
 * - Corrección ortográfica con distancia de edición
 * - Optimización para alfabetos grandes
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************
 * PARTE 1: TRIE ESTÁNDAR
 ************************************************************/
#define ALPHABET_SIZE 26 // Solo letras minúsculas a-z

// Estructura del nodo para el trie estándar
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool isEnd;                // Marca fin de palabra
} TrieNode;

// Crea un nuevo nodo para el trie estándar
TrieNode* createNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node) {
        node->isEnd = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Inserta una palabra en el trie estándar
void insert(TrieNode* root, const char* key) {
    TrieNode* node = root;
    while (*key != '\0') {
        int index = (*key - 'a');  // Asume caracteres minúsculos
        if (node->children[index] == NULL) {
            node->children[index] = createNode();
        }
        node = node->children[index];
        key++;
    }
    node->isEnd = true;  // Marca el final de la palabra
}

// Busca una palabra en el trie estándar
bool search(TrieNode* root, const char* key) {
    TrieNode* current = root;

    while (*key) {
        int index = *key - 'a';  // Asume caracteres en minúscula
        if (!current->children[index])
            return false;

        current = current->children[index];
        key++;
    }

    return (current != NULL && current->isEnd);
}

// Libera la memoria del trie estándar
void freeTrie(TrieNode* node) {
    if (node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i])
                freeTrie(node->children[i]);
        }
        free(node);
    }
}

// Función para buscar por prefijo en trie estándar
void searchByPrefix(TrieNode* root, char* prefix, char* buffer, int depth) {
    if (!root) return;

    // Si llegamos al fin de una palabra, la imprimimos
    if (root->isEnd) {
        buffer[depth] = '\0';
        printf("%s\n", buffer);
    }

    // Recorremos todos los posibles caracteres
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            buffer[depth] = 'a' + i;
            searchByPrefix(root->children[i], prefix, buffer, depth + 1);
        }
    }
}

// Función para iniciar la búsqueda por prefijo
void findWordsWithPrefix(TrieNode* root, const char* prefix) {
    TrieNode* current = root;
    char buffer[100];  // Buffer para almacenar palabras

    // Primero navegamos hasta el final del prefijo
    int prefixLen = 0;
    while (*prefix) {
        int index = *prefix - 'a';
        if (!current->children[index])
            return;  // No hay palabras con este prefijo

        buffer[prefixLen++] = *prefix;  // Guardamos el prefijo en el buffer
        current = current->children[index];
        prefix++;
    }

    // Ahora buscamos todas las palabras a partir de este punto
    searchByPrefix(current, buffer, buffer, prefixLen);
}

// Función para encontrar el nodo correspondiente al prefijo
TrieNode* findPrefixNode(TrieNode* root, const char* prefix) {
    TrieNode* current = root;

    while (*prefix) {
        int index = *prefix - 'a';
        if (!current->children[index])
            return NULL;

        current = current->children[index];
        prefix++;
    }

    return current;
}

// Función recursiva para encontrar todas las palabras a partir de un nodo
void findAllWords(TrieNode* node, char* buffer, int length) {
    if (node->isEnd) {
        buffer[length] = '\0';
        printf("Sugerencia: %s\n", buffer);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            buffer[length] = 'a' + i;
            findAllWords(node->children[i], buffer, length + 1);
        }
    }
}

// Ejemplo para autocompletado - función principal
void autocomplete(TrieNode* root, const char* prefix) {
    char buffer[100];
    strcpy(buffer, prefix);

    // Encontrar todas las palabras con el prefijo dado
    TrieNode* prefixNode = findPrefixNode(root, prefix);
    if (prefixNode) {
        // Encontrar y mostrar todas las palabras que comienzan con este prefijo
        findAllWords(prefixNode, buffer, strlen(prefix));
    } else {
        printf("No hay palabras con el prefijo '%s'\n", prefix);
    }
}

/************************************************************
 * PARTE 2: TERNARY SEARCH TRIE (TST)
 ************************************************************/

// Estructura del nodo para el TST
typedef struct TSTNode {
    char data;               // Carácter almacenado en este nodo
    bool isEndOfWord;        // Marca fin de palabra
    struct TSTNode *left;    // Caracteres lexicográficamente menores
    struct TSTNode *middle;  // Siguiente carácter de la palabra actual
    struct TSTNode *right;   // Caracteres lexicográficamente mayores
} TSTNode;

// Crear un nuevo nodo TST
TSTNode* createTSTNode(char data) {
    TSTNode* node = (TSTNode*)malloc(sizeof(TSTNode));
    if (node) {
        node->data = data;
        node->isEndOfWord = false;
        node->left = node->middle = node->right = NULL;
    }
    return node;
}

// Insertar en un TST
void insertTST(TSTNode** root, const char* word) {
    if (!*word) return;  // Si la palabra está vacía, salimos

    if (!*root)
        *root = createTSTNode(*word);  // Si el nodo no existe, lo creamos

    // Si el carácter actual es menor que el nodo, ir a la izquierda
    if (*word < (*root)->data)
        insertTST(&((*root)->left), word);

    // Si el carácter actual es mayor que el nodo, ir a la derecha
    else if (*word > (*root)->data)
        insertTST(&((*root)->right), word);

    // Si es igual, procesar el siguiente carácter
    else {
        // Si es el último carácter, marcar como fin de palabra
        if (*(word+1) == '\0')
            (*root)->isEndOfWord = true;
        else
            insertTST(&((*root)->middle), word+1);
    }
}

// Buscar en un TST
bool searchTST(TSTNode* root, const char* word) {
    if (!root) return false;  // Si el nodo no existe, la palabra no está

    // Si el carácter actual es menor que el nodo, buscar a la izquierda
    if (*word < root->data)
        return searchTST(root->left, word);

    // Si el carácter actual es mayor que el nodo, buscar a la derecha
    else if (*word > root->data)
        return searchTST(root->right, word);

    // Si es igual, continuar con el siguiente carácter
    else {
        // Si llegamos al final de la palabra
        if (*(word+1) == '\0')
            return root->isEndOfWord;  // Verificar si es fin de palabra

        // Si no es el final, seguir con el siguiente carácter
        return searchTST(root->middle, word+1);
    }
}

// Liberar la memoria del TST
void freeTST(TSTNode* root) {
    if (root) {
        freeTST(root->left);
        freeTST(root->middle);
        freeTST(root->right);
        free(root);
    }
}

// Encontrar todas las palabras en el TST con un prefijo dado
void findTSTByPrefix(TSTNode* root, char* buffer, int depth, const char* prefix, int prefixLen) {
    if (!root) return;

    // Recorrer subárbol izquierdo
    findTSTByPrefix(root->left, buffer, depth, prefix, prefixLen);

    // Procesar nodo actual
    buffer[depth] = root->data;

    // Si es fin de palabra y hemos completado al menos el prefijo
    if (root->isEndOfWord && depth >= prefixLen - 1) {
        buffer[depth + 1] = '\0';
        printf("%s\n", buffer);
    }

    // Recorrer subárbol medio (siguiente carácter)
    findTSTByPrefix(root->middle, buffer, depth + 1, prefix, prefixLen);

    // Recorrer subárbol derecho
    findTSTByPrefix(root->right, buffer, depth, prefix, prefixLen);
}

// Autocompletado en TST
void autocompleteTST(TSTNode* root, const char* prefix) {
    if (!root || !*prefix) return;

    // Navegar hasta el nodo que representa el último carácter del prefijo
    TSTNode* prefixNode = root;
    const char* pCurrent = prefix;

    while (prefixNode && *pCurrent) {
        if (*pCurrent < prefixNode->data)
            prefixNode = prefixNode->left;
        else if (*pCurrent > prefixNode->data)
            prefixNode = prefixNode->right;
        else {
            // Si encontramos el carácter actual
            if (*(pCurrent + 1) == '\0') {
                // Llegamos al último carácter del prefijo
                break;
            }
            prefixNode = prefixNode->middle;
            pCurrent++;
        }
    }

    // Si no encontramos el prefijo completo
    if (!prefixNode) {
        printf("No hay palabras con el prefijo '%s'\n", prefix);
        return;
    }

    // Encontrar todas las palabras a partir del nodo del prefijo
    char buffer[100];
    strcpy(buffer, prefix);
    int prefixLen = strlen(prefix);

    // Si el último carácter del prefijo es fin de palabra, mostrarlo
    if (prefixNode->isEndOfWord) {
        printf("%s\n", prefix);
    }

    // Mostrar todas las palabras que extienden el prefijo
    findTSTByPrefix(prefixNode->middle, buffer, prefixLen, prefix, prefixLen);
}

/************************************************************
 * PARTE 3: FUNCIONALIDADES AVANZADAS
 ************************************************************/

#define MAX_EDIT_DISTANCE 2

// Función para sugerir correcciones con distancia de edición
void suggestCorrections(TrieNode* root, const char* word) {
    int len = strlen(word);
    char buffer[100];

    // Buscar palabras con distancia de edición ≤ MAX_EDIT_DISTANCE
    findSimilarWords(root, word, buffer, 0, 0, 0, MAX_EDIT_DISTANCE);
}

// Función recursiva para encontrar palabras similares (corrección ortográfica)
void findSimilarWords(TrieNode* node, const char* word, char* buffer,
                      int bufIdx, int wordIdx, int edits, int maxEdits) {
    // Si superamos la distancia máxima, terminamos
    if (edits > maxEdits) return;

    // Si llegamos al fin de la palabra en el trie
    if (node->isEnd && wordIdx >= strlen(word)) {
        buffer[bufIdx] = '\0';
        printf("Sugerencia (%d edits): %s\n", edits, buffer);
    }

    // Explorar todas las posibles transiciones
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            char c = 'a' + i;

            // Caso 1: Coincidencia (no edición)
            if (wordIdx < strlen(word) && c == word[wordIdx]) {
                buffer[bufIdx] = c;
                findSimilarWords(node->children[i], word, buffer,
                                bufIdx+1, wordIdx+1, edits, maxEdits);
            }
            // Caso 2: Sustitución (1 edición)
            else if (wordIdx < strlen(word)) {
                buffer[bufIdx] = c;
                findSimilarWords(node->children[i], word, buffer,
                                bufIdx+1, wordIdx+1, edits+1, maxEdits);
            }
            // Caso 3: Inserción (1 edición)
            if (edits < maxEdits) {
                buffer[bufIdx] = c;
                findSimilarWords(node->children[i], word, buffer,
                                bufIdx+1, wordIdx, edits+1, maxEdits);
            }
        }
    }

    // Caso 4: Eliminación (1 edición)
    if (wordIdx < strlen(word) && edits < maxEdits) {
        findSimilarWords(node, word, buffer,
                        bufIdx, wordIdx+1, edits+1, maxEdits);
    }
}

/************************************************************
 * PARTE 4: OPTIMIZACIÓN PARA ALFABETOS GRANDES
 ************************************************************/

// Estructura para trie con mapa (para alfabetos grandes)
typedef struct TrieNodeMap {
    bool isEnd;
    struct MapEntry** children;  // Mapa dinámico
    int childCount;              // Número de hijos actuales
    int capacity;                // Capacidad del mapa
} TrieNodeMap;

// Entrada en el mapa de hijos
typedef struct MapEntry {
    int character;               // Carácter (puede ser Unicode)
    struct TrieNodeMap* node;    // Nodo hijo
} MapEntry;

// Crear nodo para trie con mapa
TrieNodeMap* createMapNode() {
    TrieNodeMap* node = (TrieNodeMap*)malloc(sizeof(TrieNodeMap));
    if (node) {
        node->isEnd = false;
        node->childCount = 0;
        node->capacity = 4;      // Capacidad inicial
        node->children = (MapEntry**)malloc(node->capacity * sizeof(MapEntry*));

        if (!node->children) {
            free(node);
            return NULL;
        }
    }
    return node;
}

// Insertar un hijo en el mapa de nodos (mantiene orden)
MapEntry* addChild(TrieNodeMap* node, int character) {
    // Buscar posición de inserción (orden binario)
    int pos = 0;
    while (pos < node->childCount &&
           ((MapEntry*)node->children[pos])->character < character) {
        pos++;
    }

    // Si el carácter ya existe, reutilizamos
    if (pos < node->childCount &&
        ((MapEntry*)node->children[pos])->character == character) {
        return node->children[pos];
    }

    // Verificar si necesitamos expandir el mapa
    if (node->childCount >= node->capacity) {
        node->capacity *= 2;
        MapEntry** newChildren = (MapEntry**)realloc(
            node->children, node->capacity * sizeof(MapEntry*));

        if (!newChildren) return NULL;
        node->children = newChildren;
    }

    // Crear nueva entrada
    MapEntry* entry = (MapEntry*)malloc(sizeof(MapEntry));
    if (!entry) return NULL;

    entry->character = character;
    entry->node = createMapNode();

    // Desplazar elementos para insertar en orden
    for (int i = node->childCount; i > pos; i--) {
        node->children[i] = node->children[i-1];
    }

    // Insertar nueva entrada
    node->children[pos] = entry;
    node->childCount++;

    return entry;
}

// Buscar un carácter en el mapa (búsqueda binaria)
MapEntry* findChild(TrieNodeMap* node, int character) {
    int left = 0, right = node->childCount - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        MapEntry* entry = (MapEntry*)node->children[mid];

        if (entry->character == character)
            return entry;
        else if (entry->character < character)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return NULL;
}

// Insertar palabra en trie con mapa
void insertMap(TrieNodeMap* root, const char* key) {
    TrieNodeMap* node = root;

    while (*key) {
        int character = (unsigned char)(*key);  // Soporte para Unicode
        MapEntry* entry = findChild(node, character);

        if (!entry) {
            entry = addChild(node, character);
        }

        node = entry->node;
        key++;
    }

    node->isEnd = true;
}

// Buscar palabra en trie con mapa
bool searchMap(TrieNodeMap* root, const char* key) {
    TrieNodeMap* node = root;

    while (*key && node) {
        int character = (unsigned char)(*key);
        MapEntry* entry = findChild(node, character);

        if (!entry) return false;
        node = entry->node;
        key++;
    }

    return (node && node->isEnd);
}

// Liberar memoria del trie con mapa
void freeMapTrie(TrieNodeMap* node) {
    if (node) {
        for (int i = 0; i < node->childCount; i++) {
            MapEntry* entry = node->children[i];
            freeMapTrie(entry->node);
            free(entry);
        }
        free(node->children);
        free(node);
    }
}

/************************************************************
 * PARTE 5: PROGRAMA PRINCIPAL
 ************************************************************/

int main(void) {
    printf("=== DEMOSTRACIÓN DE TRIE ESTÁNDAR ===\n");

    // Crear trie estándar
    TrieNode* trie = createNode();

    // Insertar palabras
    printf("Insertando palabras en el trie estándar...\n");
    insert(trie, "casa");
    insert(trie, "cama");
    insert(trie, "cara");
    insert(trie, "camino");
    insert(trie, "camisa");

    // Búsqueda de palabras
    printf("\nBúsqueda de palabras en trie estándar:\n");
    printf("'casa' está en el trie? %s\n", search(trie, "casa") ? "Sí" : "No");
    printf("'cama' está en el trie? %s\n", search(trie, "cama") ? "Sí" : "No");
    printf("'camisa' está en el trie? %s\n", search(trie, "camisa") ? "Sí" : "No");
    printf("'casona' está en el trie? %s\n", search(trie, "casona") ? "Sí" : "No");

    // Autocompletado
    printf("\nAutocompletado para prefijo 'ca' en trie estándar:\n");
    autocomplete(trie, "ca");

    printf("\n=== DEMOSTRACIÓN DE TERNARY SEARCH TRIE (TST) ===\n");

    // Crear TST
    TSTNode* tst = NULL;  // Inicialmente vacío

    // Insertar palabras
    printf("Insertando palabras en el TST...\n");
    insertTST(&tst, "casa");
    insertTST(&tst, "cama");
    insertTST(&tst, "cara");
    insertTST(&tst, "camino");
    insertTST(&tst, "camisa");

    // Búsqueda de palabras
    printf("\nBúsqueda de palabras en TST:\n");
    printf("'casa' está en el TST? %s\n", searchTST(tst, "casa") ? "Sí" : "No");
    printf("'cama' está en el TST? %s\n", searchTST(tst, "cama") ? "Sí" : "No");
    printf("'camisa' está en el TST? %s\n", searchTST(tst, "camisa") ? "Sí" : "No");
    printf("'casona' está en el TST? %s\n", searchTST(tst, "casona") ? "Sí" : "No");

    // Liberar memoria
    freeTrie(trie);
    freeTST(tst);

    printf("\nMemoria liberada correctamente.\n");
    return 0;
}