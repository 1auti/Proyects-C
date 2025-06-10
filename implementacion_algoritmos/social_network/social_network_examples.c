#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "social_network.h"

// Declarar la función de ejemplos avanzados
void ejecutarEjemplosAvanzados();

void mostrarMenu() {
    printf("\n=== SISTEMA DE ANÁLISIS DE REDES SOCIALES ===\n");
    printf("1. Ejecutar ejemplos avanzados\n");
    printf("2. Crear red personalizada\n");
    printf("3. Análisis básico de red\n");
    printf("4. Salir\n");
    printf("Selecciona una opción: ");
}

void crearRedPersonalizada() {
    printf("\n📊 CREANDO RED PERSONALIZADA\n");
    printf("============================\n");
    
    int numUsuarios, numConexiones;
    
    printf("Número de usuarios: ");
    scanf("%d", &numUsuarios);
    
    SocialNetwork* net = createSocialNetwork(numUsuarios * 2);
    
    // Crear usuarios básicos
    for (int i = 1; i <= numUsuarios; i++) {
        char username[50];
        char fullName[100];
        sprintf(username, "user_%d", i);
        sprintf(fullName, "Usuario %d", i);

        // Llamada correcta con 3 parámetros
        addUser(net, username, fullName);

        // Establecer verificación después de crear el usuario
        User* user = findUserById(net, i);
        if (user) {
            user->isVerified = (rand() % 2 == 1);
        }
    }

    printf("Número de conexiones aleatorias: ");
    scanf("%d", &numConexiones);

    // Crear conexiones aleatorias
    for (int i = 0; i < numConexiones; i++) {
        int user1 = (rand() % numUsuarios) + 1;
        int user2 = (rand() % numUsuarios) + 1;

        if (user1 != user2) {
            // Llamada correcta con 5 parámetros según tu implementación
            double strength = 0.5 + (rand() % 50) / 100.0;
            addConnection(net, user1, user2, "friend", strength);
        }
    }

    // Mostrar estadísticas básicas
    printf("\n📈 Estadísticas de la red:\n");
    printf("- Usuarios: %d\n", net->numUsers);

    int totalConexiones = 0;
    for (int i = 1; i <= numUsuarios; i++) {
        User* user = findUserById(net, i);
        if (user && user->connections) {
            totalConexiones += user->connections->size;
        }
    }

    printf("- Conexiones totales: %d\n", totalConexiones / 2);
    if (numUsuarios > 0) {
        printf("- Conexiones promedio por usuario: %.2f\n", (double)totalConexiones / numUsuarios);
    }

    // Encontrar usuarios más conectados
    printf("\n🔝 Top 3 usuarios más conectados:\n");
    int usuariosProcessados[numUsuarios];
    for (int i = 0; i < numUsuarios; i++) usuariosProcessados[i] = 0;

    for (int rank = 1; rank <= 3 && rank <= numUsuarios; rank++) {
        int maxConexiones = -1;
        User* topUser = NULL;
        int topUserId = -1;

        for (int i = 1; i <= numUsuarios; i++) {
            if (usuariosProcessados[i-1]) continue;

            User* user = findUserById(net, i);
            if (user && user->connections && user->connections->size > maxConexiones) {
                maxConexiones = user->connections->size;
                topUser = user;
                topUserId = i;
            }
        }

        if (topUser && topUserId > 0) {
            printf("%d. %s - %d conexiones\n", rank, topUser->username, maxConexiones);
            usuariosProcessados[topUserId-1] = 1;
        }
    }

    destroySocialNetwork(net);
}

void analisisBasico() {
    printf("\n🔍 ANÁLISIS BÁSICO DE RED DEMO\n");
    printf("==============================\n");

    // Crear una red pequeña de demostración
    SocialNetwork* net = createSocialNetwork(20);

    // Usuarios de ejemplo - Llamadas correctas con username y fullName
    addUser(net, "alice", "Alice Johnson");
    addUser(net, "bob", "Bob Smith");
    addUser(net, "charlie", "Charlie Brown");
    addUser(net, "diana", "Diana Prince");
    addUser(net, "eve", "Eve Adams");

    // Establecer verificación después de crear usuarios
    User* alice = findUserById(net, 1);
    User* charlie = findUserById(net, 3);
    User* eve = findUserById(net, 5);

    if (alice) alice->isVerified = true;
    if (charlie) charlie->isVerified = true;
    if (eve) eve->isVerified = true;

    // Conexiones de ejemplo - Llamadas correctas con 5 parámetros
    addConnection(net, 1, 2, "friend", 0.8);
    addConnection(net, 1, 3, "colleague", 0.6);
    addConnection(net, 2, 4, "friend", 0.9);
    addConnection(net, 3, 4, "friend", 0.7);
    addConnection(net, 3, 5, "colleague", 0.5);
    addConnection(net, 4, 5, "friend", 0.8);

    // Actualizar scores de influencia
    for (int i = 1; i <= 5; i++) {
        updateInfluenceScore(net, i);
    }

    printf("Red creada con 5 usuarios y 6 conexiones\n\n");

    // Mostrar información de cada usuario
    for (int i = 1; i <= 5; i++) {
        User* user = findUserById(net, i);
        if (user) {
            printf("👤 %s (%s):\n", user->username, user->fullName);

            if (user->connections) {
                printf("   - Conexiones: %d\n", user->connections->size);
            } else {
                printf("   - Conexiones: 0\n");
            }

            printf("   - Verificado: %s\n", user->isVerified ? "Sí" : "No");
            printf("   - Score de influencia: %.2f\n", user->influenceScore);

            printf("   - Conectado con: ");
            if (user->connections && user->connections->head) {
                ListNode* node = user->connections->head;
                while (node) {
                    int* connId = (int*)node->data;
                    User* connUser = findUserById(net, *connId);
                    if (connUser) {
                        printf("%s ", connUser->username);
                    }
                    node = node->next;
                }
            }
            printf("\n\n");
        }
    }

    // Análisis de separación
    printf("🛣️  Análisis de conectividad:\n");
    for (int i = 2; i <= 5; i++) {
        int separation = calculateSeparationDegree(net, 1, i);
        User* targetUser = findUserById(net, i);
        if (targetUser && separation >= 0) {
            printf("- Distancia de alice a %s: %d pasos\n",
                   targetUser->username, separation);
        }
    }

    destroySocialNetwork(net);
}

int main() {
    srand(time(NULL));

    printf("🌐 BIENVENIDO AL SISTEMA DE ANÁLISIS DE REDES SOCIALES\n");
    printf("=====================================================\n");

    int opcion;
    do {
        mostrarMenu();
        if (scanf("%d", &opcion) != 1) {
            printf("❌ Entrada no válida\n");
            // Limpiar buffer de entrada
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch(opcion) {
            case 1:
                ejecutarEjemplosAvanzados();
                break;
            case 2:
                crearRedPersonalizada();
                break;
            case 3:
                analisisBasico();
                break;
            case 4:
                printf("\n👋 ¡Gracias por usar el sistema!\n");
                break;
            default:
                printf("\n❌ Opción no válida\n");
        }

        if (opcion != 4) {
            printf("\nPresiona Enter para continuar...");
            // Limpiar buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar(); // Esperar Enter del usuario
        }
        
    } while(opcion != 4);
    
    return 0;
}