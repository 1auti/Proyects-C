#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definir el orden del árbol B (máximo número de hijos)
#define ORDER 5

// Estructura de un nodo del árbol B
typedef struct BTreeNode {
    int keys[ORDER-1];          // Arreglo de claves
    void* values[ORDER-1];      // Valores asociados a las claves
    struct BTreeNode* children[ORDER]; // Punteros a los hijos
    int num_keys;               // Número actual de claves
    bool is_leaf;               // Indica si es un nodo hoja
} BTreeNode;

// Estructura del árbol B
typedef struct {
    BTreeNode* root;            // Puntero a la raíz
} BTree;

// Función para crear un nuevo nodo
BTreeNode* create_node(bool is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    if (!node) {
        printf("Error: No se pudo asignar memoria\n");
        exit(1);
    }

    node->num_keys = 0;
    node->is_leaf = is_leaf;

    // Inicializar los punteros a hijos con NULL
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }

    return node;
}

// Inicializar un nuevo árbol B
BTree* init_btree() {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    if (!tree) {
        printf("Error: No se pudo asignar memoria\n");
        exit(1);
    }

    // Crear la raíz como un nodo hoja
    tree->root = create_node(true);

    return tree;
}

// Función auxiliar para buscar una posición en un nodo
int find_position(BTreeNode* node, int key) {
    int pos = 0;
    while (pos < node->num_keys && key > node->keys[pos]) {
        pos++;
    }
    return pos;
}

// Función para dividir un nodo hijo
void split_child(BTreeNode* parent, int index, BTreeNode* child) {
    // Crear un nuevo nodo para almacenar (ORDER-1)/2 claves del hijo original
    BTreeNode* new_node = create_node(child->is_leaf);
    new_node->num_keys = (ORDER-1)/2;

    // Copiar las últimas (ORDER-1)/2 claves del hijo al nuevo nodo
    for (int j = 0; j < (ORDER-1)/2; j++) {
        new_node->keys[j] = child->keys[j + (ORDER+1)/2];
        new_node->values[j] = child->values[j + (ORDER+1)/2];
    }

    // Si el hijo no es una hoja, también copiar los punteros correspondientes
    if (!child->is_leaf) {
        for (int j = 0; j < ORDER/2; j++) {
            new_node->children[j] = child->children[j + ORDER/2];
        }
    }

    // Reducir el número de claves en el hijo original
    child->num_keys = (ORDER-1)/2;

    // Mover los punteros en el padre para hacer espacio para el nuevo hijo
    for (int j = parent->num_keys; j >= index + 1; j--) {
        parent->children[j+1] = parent->children[j];
    }

    // Enlazar el nuevo hijo en el padre
    parent->children[index+1] = new_node;

    // Mover las claves en el padre para hacer espacio para la nueva clave
    for (int j = parent->num_keys-1; j >= index; j--) {
        parent->keys[j+1] = parent->keys[j];
        parent->values[j+1] = parent->values[j];
    }

    // Copiar la clave media del hijo al padre
    parent->keys[index] = child->keys[(ORDER-1)/2];
    parent->values[index] = child->values[(ORDER-1)/2];
    parent->num_keys++;
}

// Insertar una clave-valor en un nodo que no está lleno
void insert_non_full(BTreeNode* node, int key, void* value) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // Caso 1: Es un nodo hoja, simplemente insertar la clave en la posición correcta
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->values[i+1] = node->values[i];
            i--;
        }

        node->keys[i+1] = key;
        node->values[i+1] = value;
        node->num_keys++;
    } else {
        // Caso 2: No es hoja, buscar el hijo donde debería estar la clave
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        // Si el hijo está lleno, dividirlo primero
        if (node->children[i]->num_keys == ORDER-1) {
            split_child(node, i, node->children[i]);

            if (key > node->keys[i]) {
                i++;
            }
        }

        insert_non_full(node->children[i], key, value);
    }
}

// Insertar una clave-valor en el árbol B
void btree_insert(BTree* tree, int key, void* value) {
    BTreeNode* root = tree->root;

    // Caso especial: si la raíz está llena
    if (root->num_keys == ORDER-1) {
        // Crear una nueva raíz
        BTreeNode* new_root = create_node(false);
        new_root->children[0] = root;
        tree->root = new_root;

        // Dividir la antigua raíz
        split_child(new_root, 0, root);

        // Insertar la clave en el nodo apropiado
        insert_non_full(new_root, key, value);
    } else {
        // Caso normal: la raíz no está llena
        insert_non_full(root, key, value);
    }
}

// Buscar una clave en un nodo
void* search_in_node(BTreeNode* node, int key) {
    int i = 0;
    // Encontrar la primera clave mayor o igual a key
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }

    // Si la clave fue encontrada
    if (i < node->num_keys && key == node->keys[i]) {
        return node->values[i];
    }

    // Si este es un nodo hoja, la clave no existe
    if (node->is_leaf) {
        return NULL;
    }

    // Buscar en el hijo apropiado
    return search_in_node(node->children[i], key);
}

// Buscar una clave en el árbol B
void* btree_search(BTree* tree, int key) {
    if (!tree->root) {
        return NULL;
    }
    return search_in_node(tree->root, key);
}

// Imprimir un nodo con indentación (para visualización)
void print_node(BTreeNode* node, int level) {
    // Indentación según el nivel de profundidad
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Imprimir claves del nodo
    printf("Claves: ");
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");

    // Imprimir hijos recursivamente
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_node(node->children[i], level + 1);
        }
    }
}

// Imprimir el árbol B completo
void btree_print(BTree* tree) {
    printf("Árbol B:\n");
    print_node(tree->root, 0);
    printf("\n");
}

// Liberar la memoria de un nodo y sus hijos
void free_node(BTreeNode* node) {
    if (!node) return;

    // Si no es hoja, liberar primero los hijos
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node(node->children[i]);
        }
    }

    // Liberar el nodo actual
    free(node);
}

// Liberar la memoria del árbol completo
void btree_free(BTree* tree) {
    if (tree) {
        free_node(tree->root);
        free(tree);
    }
}

// Ejemplo de uso
int main() {
    BTree* tree = init_btree();

    // Insertar algunas claves
    for (int i = 1; i <= 20; i++) {
        // Crear un valor de ejemplo (aquí solo un puntero al int)
        int* value = (int*)malloc(sizeof(int));
        *value = i * 100;  // Valor = clave * 100

        btree_insert(tree, i, value);
    }

    // Imprimir el árbol
    btree_print(tree);

    // Buscar algunas claves
    int search_keys[] = {5, 15, 25};
    for (int i = 0; i < 3; i++) {
        int* result = (int*)btree_search(tree, search_keys[i]);
        if (result) {
            printf("Clave %d encontrada, valor: %d\n", search_keys[i], *result);
        } else {
            printf("Clave %d no encontrada\n", search_keys[i]);
        }
    }

    // Liberar memoria
    btree_free(tree);

    return 0;
}