#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ORDER 5  // Orden del arbol B
#define MIN_KEYS ((ORDER-1)/2)  // Minimo numero de claves en un nodo (excepto raiz)

// Simulacion de un registro en una base de datos
typedef struct {
    int id;
    char nombre[50];
    float saldo;
} Registro;

// Estructura para un nodo del arbol B
typedef struct BTreeNode {
    int num_keys;                  // Numero de claves en el nodo
    int keys[ORDER-1];             // Arreglo de claves (IDs)
    Registro* registros[ORDER-1];  // Punteros a registros en la "base de datos"
    struct BTreeNode* children[ORDER]; // Punteros a nodos hijos
    bool is_leaf;                  // Indica si es un nodo hoja
} BTreeNode;

// Estructura para el arbol B
typedef struct {
    BTreeNode* root;  // Raiz del arbol
} BTree;

// Funcion para crear un nuevo nodo
BTreeNode* create_node(bool is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    if (node == NULL) {
        fprintf(stderr, "Error de asignacion de memoria\n");
        exit(EXIT_FAILURE);
    }

    node->is_leaf = is_leaf;
    node->num_keys = 0;

    // Inicializar punteros
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
        if (i < ORDER-1) {
            node->registros[i] = NULL;
        }
    }

    return node;
}

// Inicializar un nuevo arbol B
BTree* btree_init() {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    if (tree == NULL) {
        fprintf(stderr, "Error de asignacion de memoria\n");
        exit(EXIT_FAILURE);
    }

    // La raiz inicial es una hoja
    tree->root = create_node(true);

    return tree;
}

// Buscar una posicion en un nodo
int find_key_position(BTreeNode* node, int key) {
    int idx = 0;
    // Encontrar la primera posicion donde key <= node->keys[idx]
    while (idx < node->num_keys && key > node->keys[idx]) {
        idx++;
    }
    return idx;
}

// Dividir un nodo hijo
void split_child(BTreeNode* parent, int idx) {
    BTreeNode* new_node = create_node(parent->children[idx]->is_leaf);
    BTreeNode* child = parent->children[idx];

    // Configurar el nuevo nodo
    new_node->num_keys = MIN_KEYS;

    // Copiar las claves superiores al nuevo nodo
    for (int j = 0; j < MIN_KEYS; j++) {
        new_node->keys[j] = child->keys[j + MIN_KEYS + 1];
        new_node->registros[j] = child->registros[j + MIN_KEYS + 1];
    }

    // Si no es hoja, copiar tambien los hijos
    if (!child->is_leaf) {
        for (int j = 0; j <= MIN_KEYS; j++) {
            new_node->children[j] = child->children[j + MIN_KEYS + 1];
        }
    }

    // Actualizar el numero de claves del hijo original
    child->num_keys = MIN_KEYS;

    // Hacer espacio para el nuevo hijo en el padre
    for (int j = parent->num_keys; j > idx; j--) {
        parent->children[j+1] = parent->children[j];
    }

    // Enlazar el nuevo hijo
    parent->children[idx+1] = new_node;

    // Mover claves en el padre para hacer espacio a la clave media
    for (int j = parent->num_keys-1; j >= idx; j--) {
        parent->keys[j+1] = parent->keys[j];
        parent->registros[j+1] = parent->registros[j];
    }

    // Copiar la clave media del hijo al padre
    parent->keys[idx] = child->keys[MIN_KEYS];
    parent->registros[idx] = child->registros[MIN_KEYS];
    parent->num_keys++;
}

// Insertar una clave en un nodo no lleno
void insert_non_full(BTreeNode* node, int key, Registro* registro) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // En un nodo hoja, simplemente insertar en la posicion correcta
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

        // Si el hijo esta lleno, dividirlo primero
        if (node->children[i]->num_keys == ORDER-1) {
            split_child(node, i);

            // Despues de la division, la clave media esta en el padre
            // Determinar que hijo debe recibir la clave
            if (key > node->keys[i]) {
                i++;
            }
        }

        insert_non_full(node->children[i], key, registro);
    }
}

// Insertar un registro en el arbol B
void btree_insert(BTree* tree, int key, Registro* registro) {
    BTreeNode* root = tree->root;

    // Si la raiz esta llena, crear una nueva raiz
    if (root->num_keys == ORDER-1) {
        BTreeNode* new_root = create_node(false);
        tree->root = new_root;
        new_root->children[0] = root;

        // Dividir la antigua raiz y mover una clave a la nueva raiz
        split_child(new_root, 0);

        // Ahora la nueva raiz tiene una clave, decidir que hijo recibe la nueva clave
        int i = 0;
        if (key > new_root->keys[0]) {
            i++;
        }
        insert_non_full(new_root->children[i], key, registro);
    } else {
        // Si la raiz no esta llena, insertar normalmente
        insert_non_full(root, key, registro);
    }
}

// Buscar un registro por su clave
Registro* search_node(BTreeNode* node, int key) {
    int i = 0;

    // Encontrar la primera clave mayor o igual
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }

    // Si la clave coincide exactamente
    if (i < node->num_keys && key == node->keys[i]) {
        return node->registros[i];
    }

    // Si es un nodo hoja, la clave no existe
    if (node->is_leaf) {
        return NULL;
    }

    // Continuar la busqueda en el hijo adecuado
    return search_node(node->children[i], key);
}

// Buscar un registro en el arbol B
Registro* btree_search(BTree* tree, int key) {
    if (!tree->root) {
        return NULL;
    }

    return search_node(tree->root, key);
}

// Imprimir un nodo con indentacion
void print_node(BTreeNode* node, int level) {
    // Indentacion segun el nivel
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Imprimir las claves
    printf("Claves: ");
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }

    // Imprimir detalles de los registros
    printf("(Registros: ");
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
    printf(")\n");

    // Recursivamente imprimir los nodos hijos
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_node(node->children[i], level + 1);
        }
    }
}

// Imprimir el arbol completo
void btree_print(BTree* tree) {
    if (!tree->root) {
        printf("Arbol vacio\n");
        return;
    }

    printf("Estructura del Arbol B:\n");
    print_node(tree->root, 0);
    printf("\n");
}

// Liberar la memoria de un nodo y sus hijos
void free_node(BTreeNode* node, bool free_records) {
    if (!node) return;

    // Si no es una hoja, liberar los hijos primero
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node(node->children[i], free_records);
        }
    }

    // Opcionalmente liberar los registros (depende si son propios del arbol)
    if (free_records) {
        for (int i = 0; i < node->num_keys; i++) {
            if (node->registros[i]) {
                free(node->registros[i]);
            }
        }
    }

    // Liberar el nodo
    free(node);
}

// Liberar el arbol completo
void btree_free(BTree* tree, bool free_records) {
    if (tree) {
        free_node(tree->root, free_records);
        free(tree);
    }
}

// Recorrer el arbol en inorden y aplicar una funcion a cada registro
void traverse_inorder(BTreeNode* node, void (*func)(Registro*)) {
    if (!node) return;

    int i;
    // Para cada clave en el nodo
    for (i = 0; i < node->num_keys; i++) {
        // Primero visitar el hijo izquierdo
        if (!node->is_leaf) {
            traverse_inorder(node->children[i], func);
        }

        // Luego procesar la clave actual
        if (node->registros[i]) {
            func(node->registros[i]);
        }
    }

    // Visitar el ultimo hijo (derecho)
    if (!node->is_leaf) {
        traverse_inorder(node->children[i], func);
    }
}

// Recorrer el arbol y aplicar una funcion a cada registro
void btree_foreach(BTree* tree, void (*func)(Registro*)) {
    if (tree && tree->root) {
        traverse_inorder(tree->root, func);
    }
}

// Funcion de ejemplo para imprimir un registro
void print_registro(Registro* registro) {
    if (registro) {
        printf("ID: %d, Nombre: %s, Saldo: %.2f\n",
               registro->id, registro->nombre, registro->saldo);
    }
}

// Simulacion de una "tabla" de registros
Registro* crear_registro(int id, const char* nombre, float saldo) {
    Registro* nuevo = (Registro*)malloc(sizeof(Registro));
    if (!nuevo) {
        fprintf(stderr, "Error de asignacion de memoria\n");
        exit(EXIT_FAILURE);
    }

    nuevo->id = id;
    strncpy(nuevo->nombre, nombre, 49);
    nuevo->nombre[49] = '\0';  // Asegurar terminacion NULL
    nuevo->saldo = saldo;

    return nuevo;
}

// Ejemplo de uso simulando una base de datos indexada
int main() {
    // Crear un arbol B como indice
    BTree* indice = btree_init();

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

    // Insertar registros en el indice
    printf("Insertando registros en el indice...\n");
    for (int i = 0; i < sizeof(datos)/sizeof(datos[0]); i++) {
        Registro* reg = crear_registro(datos[i].id, datos[i].nombre, datos[i].saldo);
        btree_insert(indice, reg->id, reg);
    }

    // Imprimir la estructura del arbol
    printf("\nEstructura del indice despues de insertar todos los registros:\n");
    btree_print(indice);

    // Simular una consulta SELECT
    printf("\nConsultando registros...\n");

    int ids_buscar[] = {1250, 1500, 2000};  // El ultimo no existe
    for (int i = 0; i < 3; i++) {
        printf("\nSELECT * FROM tabla WHERE id = %d\n", ids_buscar[i]);
        Registro* resultado = btree_search(indice, ids_buscar[i]);

        if (resultado) {
            printf("Registro encontrado: ID: %d, Nombre: %s, Saldo: %.2f\n",
                   resultado->id, resultado->nombre, resultado->saldo);
        } else {
            printf("Registro con ID %d no encontrado\n", ids_buscar[i]);
        }
    }

    // Simular un UPDATE
    printf("\nActualizando un registro...\n");
    printf("UPDATE tabla SET saldo = 8000 WHERE id = 1500\n");

    Registro* reg_actualizar = btree_search(indice, 1500);
    if (reg_actualizar) {
        reg_actualizar->saldo = 8000.0;
        printf("Registro actualizado. Nuevo saldo: %.2f\n", reg_actualizar->saldo);
    }

    // Listar todos los registros (simulando un SELECT *)
    printf("\nListando todos los registros (SELECT * FROM tabla):\n");
    btree_foreach(indice, print_registro);

    // Liberar memoria
    btree_free(indice, true);  // true para liberar tambien los registros

    return 0;
}