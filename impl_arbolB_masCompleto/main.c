#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDER 5  // Orden del árbol B (número máximo de hijos)
#define MIN_KEYS ((ORDER-1)/2)  // Número mínimo de claves en un nodo (excepto raíz)

// Estructura de un nodo del árbol B
typedef struct BTreeNode {
    int num_keys;               // Número actual de claves
    int keys[ORDER-1];          // Arreglo de claves
    void* values[ORDER-1];      // Valores asociados
    struct BTreeNode* children[ORDER]; // Hijos
    bool is_leaf;               // Indica si es un nodo hoja
} BTreeNode;

// Estructura del árbol B
typedef struct {
    BTreeNode* root;            // Raíz del árbol
    int height;                 // Altura del árbol
} BTree;

// Crear un nuevo nodo
BTreeNode* create_node(bool is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    if (!node) {
        fprintf(stderr, "Error de asignación de memoria\n");
        exit(EXIT_FAILURE);
    }

    node->num_keys = 0;
    node->is_leaf = is_leaf;

    // Inicializar punteros a NULL
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }

    return node;
}

// Inicializar un árbol B vacío
BTree* btree_init() {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    if (!tree) {
        fprintf(stderr, "Error de asignación de memoria\n");
        exit(EXIT_FAILURE);
    }

    tree->root = create_node(true);
    tree->height = 1;  // Un árbol vacío tiene altura 1 (solo raíz)

    return tree;
}

// Buscar una clave en un nodo
int find_key(BTreeNode* node, int key) {
    int idx = 0;

    // Encontrar la posición de la clave
    while (idx < node->num_keys && node->keys[idx] < key) {
        idx++;
    }

    return idx;
}

// Dividir un hijo
void split_child(BTreeNode* parent, int index) {
    BTreeNode* child = parent->children[index];
    BTreeNode* new_child = create_node(child->is_leaf);

    // Configurar el nuevo hijo
    new_child->num_keys = MIN_KEYS;

    // Copiar las claves superiores al nuevo hijo
    for (int j = 0; j < MIN_KEYS; j++) {
        new_child->keys[j] = child->keys[j + MIN_KEYS + 1];
        new_child->values[j] = child->values[j + MIN_KEYS + 1];
    }

    // Si no es hoja, copiar también los punteros a los hijos
    if (!child->is_leaf) {
        for (int j = 0; j <= MIN_KEYS; j++) {
            new_child->children[j] = child->children[j + MIN_KEYS + 1];
        }
    }

    // Reducir el número de claves en el hijo original
    child->num_keys = MIN_KEYS;

    // Mover los hijos en el padre para hacer lugar al nuevo hijo
    for (int j = parent->num_keys; j >= index + 1; j--) {
        parent->children[j+1] = parent->children[j];
    }

    // Enlazar el nuevo hijo
    parent->children[index+1] = new_child;

    // Mover las claves en el padre para hacer lugar a la clave media
    for (int j = parent->num_keys-1; j >= index; j--) {
        parent->keys[j+1] = parent->keys[j];
        parent->values[j+1] = parent->values[j];
    }

    // Copiar la clave media del hijo al padre
    parent->keys[index] = child->keys[MIN_KEYS];
    parent->values[index] = child->values[MIN_KEYS];
    parent->num_keys++;
}

// Insertar en un nodo que no está lleno
void insert_non_full(BTreeNode* node, int key, void* value) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // Mover claves mayores a la derecha
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->values[i+1] = node->values[i];
            i--;
        }

        // Insertar la nueva clave
        node->keys[i+1] = key;
        node->values[i+1] = value;
        node->num_keys++;
    } else {
        // Encontrar el hijo donde debe insertarse
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        // Si el hijo está lleno, dividirlo
        if (node->children[i]->num_keys == ORDER-1) {
            split_child(node, i);

            // Determinar qué hijo debe recibir la clave
            if (key > node->keys[i]) {
                i++;
            }
        }

        insert_non_full(node->children[i], key, value);
    }
}





// Insertar una clave en el árbol B
void btree_insert(BTree* tree, int key, void* value) {
    BTreeNode* root = tree->root;

    // Caso especial: si la raíz está llena
    if (root->num_keys == ORDER-1) {
        BTreeNode* new_root = create_node(false);
        tree->root = new_root;
        new_root->children[0] = root;

        split_child(new_root, 0);

        // Ahora la raíz tiene al menos una clave, insertamos normalmente
        insert_non_full(new_root, key, value);

        // Incrementar la altura del árbol
        tree->height++;
    } else {
        insert_non_full(root, key, value);
    }
}

// Función auxiliar: encuentra el predecesor de una clave en un nodo
int get_predecessor(BTreeNode* node, int idx) {
    // Descender al hijo más a la derecha
    BTreeNode* cur = node->children[idx];
    while (!cur->is_leaf) {
        cur = cur->children[cur->num_keys];
    }

    // Retornar la última clave
    return cur->keys[cur->num_keys-1];
}

// Función auxiliar: encuentra el sucesor de una clave en un nodo
int get_successor(BTreeNode* node, int idx) {
    // Descender al hijo más a la izquierda
    BTreeNode* cur = node->children[idx+1];
    while (!cur->is_leaf) {
        cur = cur->children[0];
    }

    // Retornar la primera clave
    return cur->keys[0];
}

// Función auxiliar: fusionar dos nodos
void merge_nodes(BTreeNode* parent, int idx, BTreeNode* left, BTreeNode* right) {
    // Mover la clave del padre al nodo izquierdo
    left->keys[left->num_keys] = parent->keys[idx];
    left->values[left->num_keys] = parent->values[idx];

    // Copiar todas las claves y valores del nodo derecho al izquierdo
    for (int i = 0; i < right->num_keys; i++) {
        left->keys[left->num_keys + 1 + i] = right->keys[i];
        left->values[left->num_keys + 1 + i] = right->values[i];
    }

    // Si no son hojas, copiar también los punteros a hijos
    if (!left->is_leaf) {
        for (int i = 0; i <= right->num_keys; i++) {
            left->children[left->num_keys + 1 + i] = right->children[i];
        }
    }

    // Actualizar el número de claves en el nodo izquierdo
    left->num_keys = left->num_keys + 1 + right->num_keys;

    // Mover claves y hijos en el padre para llenar el hueco
    for (int i = idx; i < parent->num_keys-1; i++) {
        parent->keys[i] = parent->keys[i+1];
        parent->values[i] = parent->values[i+1];
    }

    for (int i = idx+1; i < parent->num_keys; i++) {
        parent->children[i] = parent->children[i+1];
    }

    // Decrementar el número de claves en el padre
    parent->num_keys--;

    // Liberar la memoria del nodo derecho
    free(right);
}

// Función auxiliar: redistribuir claves entre hermanos
void borrow_from_prev(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx-1];

    // Mover todas las claves en el hijo un lugar a la derecha
    for (int i = child->num_keys-1; i >= 0; i--) {
        child->keys[i+1] = child->keys[i];
        child->values[i+1] = child->values[i];
    }

    // Si no es hoja, mover también los punteros a hijos
    if (!child->is_leaf) {
        for (int i = child->num_keys; i >= 0; i--) {
            child->children[i+1] = child->children[i];
        }
    }

    // Copiar la clave y el valor del padre al hijo
    child->keys[0] = node->keys[idx-1];
    child->values[0] = node->values[idx-1];

    // Mover la última clave del hermano al padre
    node->keys[idx-1] = sibling->keys[sibling->num_keys-1];
    node->values[idx-1] = sibling->values[sibling->num_keys-1];

    // Actualizar el número de claves
    child->num_keys++;
    sibling->num_keys--;
}

// Función auxiliar: eliminar una clave del árbol B
void btree_remove_key(BTreeNode* node, int key) {
    int idx = find_key(node, key);
    // Si la clave está en el nodo hoja
    if (node->is_leaf) {
        for (int i = idx; i < node->num_keys - 1; i++) {
            node->keys[i] = node->keys[i+1];
            node->values[i] = node->values[i+1];
        }
        node->num_keys--;
    } else {
        // Si el nodo no es hoja, buscamos la clave
        if (node->children[idx]->num_keys >= MIN_KEYS) {
            node->keys[idx] = get_predecessor(node, idx);
            btree_remove_key(node->children[idx], node->keys[idx]);
        } else if (node->children[idx+1]->num_keys >= MIN_KEYS) {
            node->keys[idx] = get_successor(node, idx);
            btree_remove_key(node->children[idx+1], node->keys[idx]);
        } else {
            merge_nodes(node, idx, node->children[idx], node->children[idx+1]);
            btree_remove_key(node->children[idx], key);
        }
    }
}

// Función de prueba
void print_btree(BTreeNode* root, int level) {
    if (root == NULL) return;
    printf("Nivel %d: ", level);
    for (int i = 0; i < root->num_keys; i++) {
        printf("%d ", root->keys[i]);
    }
    printf("\n");
    if (!root->is_leaf) {
        for (int i = 0; i <= root->num_keys; i++) {
            print_btree(root->children[i], level + 1);
        }
    }
}

int main() {
    BTree* tree = btree_init();

    // Insertar algunas claves
    btree_insert(tree, 10, NULL);
    btree_insert(tree, 20, NULL);
    btree_insert(tree, 5, NULL);
    btree_insert(tree, 6, NULL);
    btree_insert(tree, 12, NULL);

    // Mostrar el árbol
    print_btree(tree->root, 0);

    return 0;
}
