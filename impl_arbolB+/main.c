#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ORDER 5  // Orden del arbol B+
#define MIN_KEYS ((ORDER-1)/2)  // Minimo numero de claves en un nodo (excepto raiz)

// Simulacion de un registro en una base de datos
typedef struct {
    int id;
    char nombre[50];
    float saldo;
} Registro;

// Estructura para un nodo del arbol B+
typedef struct BPlusTreeNode {
    int num_keys;                  // Numero de claves en el nodo
    int keys[ORDER-1];             // Arreglo de claves (IDs)
    struct BPlusTreeNode* children[ORDER]; // Punteros a nodos hijos
    bool is_leaf;                  // Indica si es un nodo hoja

    // Componentes especificos de nodos hoja
    Registro* registros[ORDER-1];  // Solo los nodos hoja almacenan registros
    struct BPlusTreeNode* next;    // Puntero al siguiente nodo hoja (solo en hojas)
} BPlusTreeNode;

// Estructura para el arbol B+
typedef struct {
    BPlusTreeNode* root;           // Raiz del arbol
    BPlusTreeNode* leftmost_leaf;  // Puntero a la hoja más izquierda (para recorridos)
} BPlusTree;

// Funcion para crear un nuevo nodo
BPlusTreeNode* create_node(bool is_leaf) {
    BPlusTreeNode* node = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
    if (node == NULL) {
        fprintf(stderr, "Error de asignacion de memoria\n");
        exit(EXIT_FAILURE);
    }

    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;  // Solo se usara en nodos hoja

    // Inicializar punteros
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
        if (i < ORDER-1) {
            node->registros[i] = NULL;
        }
    }

    return node;
}

// Inicializar un nuevo arbol B+
BPlusTree* bplustree_init() {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    if (tree == NULL) {
        fprintf(stderr, "Error de asignacion de memoria\n");
        exit(EXIT_FAILURE);
    }

    // La raiz inicial es una hoja
    tree->root = create_node(true);
    tree->leftmost_leaf = tree->root;  // La raiz inicial también es la hoja más izquierda

    return tree;
}

// Buscar una posicion en un nodo
int find_key_position(BPlusTreeNode* node, int key) {
    int idx = 0;
    while (idx < node->num_keys && key > node->keys[idx]) {
        idx++;
    }
    return idx;
}

// Dividir un nodo hijo (la implementacion es diferente para B+)
void split_child(BPlusTreeNode* parent, int idx) {
    BPlusTreeNode* child = parent->children[idx];
    BPlusTreeNode* new_node = create_node(child->is_leaf);

    // Configurar el nuevo nodo
    new_node->num_keys = MIN_KEYS;

    // Copiar claves superiores al nuevo nodo
    for (int j = 0; j < MIN_KEYS; j++) {
        new_node->keys[j] = child->keys[j + MIN_KEYS];

        // Si es hoja, copiar también los registros
        if (child->is_leaf) {
            new_node->registros[j] = child->registros[j + MIN_KEYS];
        }
    }

    // Si es un nodo hoja, necesitamos mantener la clave de promoción en el nodo hoja también
    if (child->is_leaf) {
        // Ajustar el enlace de nodos hoja
        new_node->next = child->next;
        child->next = new_node;

        // Ajustar el número de claves en el hijo original
        child->num_keys = MIN_KEYS;
    } else {
        // Si no es hoja, movemos los punteros a hijos
        for (int j = 0; j <= MIN_KEYS; j++) {
            new_node->children[j] = child->children[j + MIN_KEYS];
        }

        // Ajustar el número de claves en el hijo original
        child->num_keys = MIN_KEYS - 1;
    }

    // Hacer espacio para el nuevo hijo en el padre
    for (int j = parent->num_keys; j > idx; j--) {
        parent->children[j+1] = parent->children[j];
    }

    // Enlazar el nuevo nodo como hijo del padre
    parent->children[idx+1] = new_node;

    // Mover las claves en el padre para hacer espacio
    for (int j = parent->num_keys-1; j >= idx; j--) {
        parent->keys[j+1] = parent->keys[j];
    }

    // Clave a promover al padre - diferente en B+
    // En B+ usamos la primera clave del nodo derecho
    parent->keys[idx] = new_node->keys[0];
    parent->num_keys++;
}

// Insertar una clave en un nodo no lleno
void insert_non_full(BPlusTreeNode* node, int key, Registro* registro) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // En un nodo hoja, simplemente insertar en la posición correcta
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->registros[i+1] = node->registros[i];
            i--;
        }

        node->keys[i+1] = key;
        node->registros[i+1] = registro;
        node->num_keys++;
    } else {
        // En un nodo interno, encontrar el hijo adecuado
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        // Si el hijo está lleno, dividirlo primero
        if (node->children[i]->num_keys == ORDER-1) {
            split_child(node, i);

            // Después de la división, la nueva clave podría ir en otro hijo
            if (key >= node->keys[i]) {
                i++;
            }
        }

        insert_non_full(node->children[i], key, registro);
    }
}

// Insertar un registro en el árbol B+
void bplustree_insert(BPlusTree* tree, int key, Registro* registro) {
    BPlusTreeNode* root = tree->root;

    // Si la raíz está llena, crear una nueva raíz
    if (root->num_keys == ORDER-1) {
        BPlusTreeNode* new_root = create_node(false);
        tree->root = new_root;
        new_root->children[0] = root;

        split_child(new_root, 0);

        // Decidir qué hijo debe recibir la nueva clave
        int i = 0;
        if (key >= new_root->keys[0]) {
            i++;
        }
        insert_non_full(new_root->children[i], key, registro);

        // Si la raíz inicial era la hoja más a la izquierda, actualizar el puntero
        if (tree->leftmost_leaf == root) {
            tree->leftmost_leaf = new_root->children[0];
        }
    } else {
        insert_non_full(root, key, registro);
    }
}

// Buscar un registro por su clave
Registro* search_node(BPlusTreeNode* node, int key) {
    int i = 0;

    // Encontrar la primera clave mayor o igual
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }

    // Si es un nodo hoja
    if (node->is_leaf) {
        // Si la clave coincide exactamente
        if (i < node->num_keys && key == node->keys[i]) {
            return node->registros[i];
        } else {
            return NULL;  // No se encontró
        }
    }

    // Si es un nodo interno, seguir buscando en el hijo adecuado
    return search_node(node->children[i], key);
}

// Buscar un registro en el árbol B+
Registro* bplustree_search(BPlusTree* tree, int key) {
    if (!tree->root) {
        return NULL;
    }

    return search_node(tree->root, key);
}

// Función para buscar registros en un rango (ventaja clave de B+)
void range_search(BPlusTree* tree, int lower, int upper, void (*process)(Registro*)) {
    if (!tree->root) return;

    // Primero, encontrar el nodo hoja que contiene la clave inferior
    BPlusTreeNode* node = tree->root;

    // Descender hasta la hoja
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && lower > node->keys[i]) {
            i++;
        }
        node = node->children[i];
    }

    // Encontrar la posición inicial en la hoja
    int i = 0;
    while (i < node->num_keys && lower > node->keys[i]) {
        i++;
    }

    // Recorrer las hojas secuencialmente hasta encontrar la clave superior
    while (node != NULL) {
        for (; i < node->num_keys; i++) {
            if (node->keys[i] > upper) {
                return;  // Terminamos si superamos la clave superior
            }

            // Procesar el registro
            if (node->registros[i]) {
                process(node->registros[i]);
            }
        }

        // Pasar al siguiente nodo hoja
        node = node->next;
        i = 0;  // Reiniciar el índice para el nuevo nodo
    }
}

// Imprimir un nodo con indentación
void print_node(BPlusTreeNode* node, int level) {
    // Indentación según el nivel
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Imprimir las claves
    printf("Claves: ");
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }

    // Si es hoja, mostrar los registros
    if (node->is_leaf) {
        printf("(Hoja - Registros: ");
        for (int i = 0; i < node->num_keys; i++) {
            if (node->registros[i]) {
                printf("[ID:%d, %s, %.2f] ",
                       node->registros[i]->id,
                       node->registros[i]->nombre,
                       node->registros[i]->saldo);
            } else {
                printf("[NULL] ");
            }
        }

        printf("-> %p)", (void*)node->next);
    } else {
        printf("(Interno)");
    }
    printf("\n");

    // Recursivamente imprimir los nodos hijos
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_node(node->children[i], level + 1);
        }
    }
}

// Imprimir el árbol B+ completo
void bplustree_print(BPlusTree* tree) {
    if (!tree->root) {
        printf("Arbol vacio\n");
        return;
    }

    printf("Estructura del Arbol B+:\n");
    print_node(tree->root, 0);
    printf("\n");
}

// Mostrar los nodos hoja en secuencia (característica única de B+)
void print_leaves(BPlusTree* tree) {
    if (!tree->root) {
        printf("Arbol vacio\n");
        return;
    }

    printf("Nodos hoja en secuencia:\n");
    BPlusTreeNode* leaf = tree->leftmost_leaf;
    int leaf_num = 1;

    while (leaf) {
        printf("Hoja %d: ", leaf_num++);
        for (int i = 0; i < leaf->num_keys; i++) {
            printf("%d ", leaf->keys[i]);
        }
        printf("\n");
        leaf = leaf->next;
    }
    printf("\n");
}

// Liberar la memoria de un nodo y sus hijos
void free_node(BPlusTreeNode* node, bool free_records) {
    if (!node) return;

    // Si no es una hoja, liberar los hijos primero
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node(node->children[i], free_records);
        }
    } else if (free_records) {
        // Liberar los registros solo si es hoja (en B+ solo las hojas tienen registros)
        for (int i = 0; i < node->num_keys; i++) {
            if (node->registros[i]) {
                free(node->registros[i]);
            }
        }
    }

    // Liberar el nodo
    free(node);
}

// Liberar el árbol B+ completo
void bplustree_free(BPlusTree* tree, bool free_records) {
    if (tree) {
        free_node(tree->root, free_records);
        free(tree);
    }
}

// Función para imprimir un registro
void print_registro(Registro* registro) {
    if (registro) {
        printf("ID: %d, Nombre: %s, Saldo: %.2f\n",
               registro->id, registro->nombre, registro->saldo);
    }
}

// Crear un nuevo registro
Registro* crear_registro(int id, const char* nombre, float saldo) {
    Registro* nuevo = (Registro*)malloc(sizeof(Registro));
    if (!nuevo) {
        fprintf(stderr, "Error de asignacion de memoria\n");
        exit(EXIT_FAILURE);
    }

    nuevo->id = id;
    strncpy(nuevo->nombre, nombre, 49);
    nuevo->nombre[49] = '\0';
    nuevo->saldo = saldo;

    return nuevo;
}

// Ejemplo de uso simulando una base de datos indexada
int main() {
    // Crear un árbol B+ como índice
    BPlusTree* indice = bplustree_init();

    // Datos de ejemplo
    struct {
        int id;
        const char* nombre;
        float saldo;
    } datos[] = {
        {1001, "Juan Perez", 5000.0},
        {1500, "Ana Garcia", 7500.0},
        {1250, "Carlos Lopez", 3200.0},
        {1800, "Maria Rodriguez", 9100.0},
        {1100, "Pedro Sanchez", 4300.0},
        {1350, "Lucia Martinez", 6200.0},
        {1650, "Sergio Gomez", 8100.0},
        {1075, "Elena Torres", 3900.0},
    };

    // Insertar registros en el índice
    printf("Insertando registros en el indice B+...\n");
    for (int i = 0; i < sizeof(datos)/sizeof(datos[0]); i++) {
        Registro* reg = crear_registro(datos[i].id, datos[i].nombre, datos[i].saldo);
        bplustree_insert(indice, reg->id, reg);
    }

    // Imprimir la estructura del árbol B+
    printf("\nEstructura del indice B+ despues de insertar todos los registros:\n");
    bplustree_print(indice);

    // Mostrar los nodos hoja en secuencia (característica única de B+)
    print_leaves(indice);

    // Simular una consulta SELECT
    printf("\nConsultando registros individuales...\n");
    int ids_buscar[] = {1250, 1500, 2000};
    for (int i = 0; i < 3; i++) {
        printf("\nSELECT * FROM tabla WHERE id = %d\n", ids_buscar[i]);
        Registro* resultado = bplustree_search(indice, ids_buscar[i]);

        if (resultado) {
            printf("Registro encontrado: ID: %d, Nombre: %s, Saldo: %.2f\n",
                   resultado->id, resultado->nombre, resultado->saldo);
        } else {
            printf("Registro con ID %d no encontrado\n", ids_buscar[i]);
        }
    }

    // Búsqueda por rango (ventaja principal de B+)
    printf("\nConsultando por rango (caracteristica principal de B+)...\n");
    printf("SELECT * FROM tabla WHERE id BETWEEN 1100 AND 1400\n");
    printf("Resultados:\n");
    range_search(indice, 1100, 1400, print_registro);

    // Simular un UPDATE
    printf("\nActualizando un registro...\n");
    printf("UPDATE tabla SET saldo = 8000 WHERE id = 1500\n");
    Registro* reg_actualizar = bplustree_search(indice, 1500);
    if (reg_actualizar) {
        reg_actualizar->saldo = 8000.0;
        printf("Registro actualizado. Nuevo saldo: %.2f\n", reg_actualizar->saldo);
    }

    // Liberar memoria
    bplustree_free(indice, true);

    return 0;
}