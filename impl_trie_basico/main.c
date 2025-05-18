#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ALPHABET_SIZE 26 // Solo letras minusculas a-z

// Estructura del nodo para el trie
typedef struct TrieNode {
 struct TrieNode * children[ALPHABET_SIZE];
    bool isEnd;
}TrieNode;

// Creamos un nodo
TrieNode * createNode() {
    TrieNode * node = (TrieNode *)malloc(sizeof(TrieNode));
    if (node) {
        node->isEnd = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Insertamos una palabra en el trie
void insertar(TrieNode * root, const char * key) {
    TrieNode * node = root;
    while (*key != '\0') {
        int index = (*key - 'a'); // asume caracteres minusculos
        if (node->children[index] == NULL) {
            node->children[index] = createNode();
        }
        node = node->children[index];
        key++;
    }
    node->isEnd = true; // Marcar el final de la palabra
}


// Busca una palabra en el trie
bool search(TrieNode* root, const char* key) {
    TrieNode* current = root;

    while (*key) {
        int index = *key - 'a';  //asume caracteres en minuscula
        if (!current->children[index])
            return false;

        current = current->children[index];
        key++;
    }

    return (current != NULL && current->isEnd);
}

// Libera la memoria del trie
void freeTrie(TrieNode* node) {
    if (node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i])
                freeTrie(node->children[i]);
        }
        free(node);
    }
}


// Funcion para buscar por prefijo
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

// Funcion para iniciar la busqueda por prefijo
void findWordsWithPrefix(TrieNode* root, const char* prefix) {
    TrieNode* current = root;
    char buffer[100]; // Buffer para almacenar palabras

    // Primero navegamos hasta el final del prefijo
    while (*prefix) {
        int index = *prefix - 'a';
        if (!current->children[index])
            return; // No hay palabras con este prefijo

        buffer[current - root] = *prefix; // Guardamos el prefijo en el buffer
        current = current->children[index];
        prefix++;
    }

    // Ahora buscamos todas las palabras a partir de este punto
    searchByPrefix(current, prefix, buffer, strlen(buffer));
}

// Funcion para encontrar el nodo correspondiente al prefijo
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

// Funcion recursiva para encontrar todas las palabras a partir de un nodo
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

// Ejemplo para autocompletado - funcion principal
void autocomplete(TrieNode* root, const char* prefix) {
    char buffer[100];
    strcpy(buffer, prefix);

    // Encontrar todas las palabras con el prefijo dado
    TrieNode* prefixNode = findPrefixNode(root, prefix);
    if (prefixNode) {
        // Encontrar y mostrar todas las palabras que comienzan con este prefijo
        findAllWords(prefixNode, buffer, strlen(prefix));
    }
}


// Funcion main para probar el codigo
int main(void) {
    // Crear el trie
    TrieNode* root = createNode();

    // Insertar palabras
    printf("Insertando palabras en el trie...\n");
    insertar(root, "casa");
    insertar(root, "cama");
    insertar(root, "cara");
    insertar(root, "camino");
    insertar(root, "camisa");
    insertar(root, "carro");
    insertar(root, "perro");
    insertar(root, "pera");
    insertar(root, "programa");
    insertar(root, "programacion");

    // Buscar palabras
    printf("\nBuscando palabras:\n");
    printf("'casa' esta en el trie? %s\n", search(root, "casa") ? "Si" : "No");
    printf("'cama' esta en el trie? %s\n", search(root, "cama") ? "Si" : "No");
    printf("'camisa' esta en el trie? %s\n", search(root, "camisa") ? "Si" : "No");
    printf("'camino' esta en el trie? %s\n", search(root, "camino") ? "Si" : "No");
    printf("'casona' esta en el trie? %s\n", search(root, "casona") ? "Si" : "No");
    printf("'programacion' esta en el trie? %s\n", search(root, "programacion") ? "Si" : "No");

    // Probar autocompletado
    printf("\nAutocompletado para prefijo 'ca':\n");
    autocomplete(root, "ca");

    printf("\nAutocompletado para prefijo 'p':\n");
    autocomplete(root, "p");

    printf("\nAutocompletado para prefijo 'pro':\n");
    autocomplete(root, "pro");

    // Autocompletado para un prefijo que no existe
    printf("\nAutocompletado para prefijo 'z':\n");
    autocomplete(root, "z");

    // Liberar memoria
    freeTrie(root);
    printf("\nMemoria liberada correctamente.\n");

    return 0;
}