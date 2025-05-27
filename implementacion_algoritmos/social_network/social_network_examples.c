//
// Created by administrador on 5/27/25.
//

#include "social_network.h"

// ===================================================================
// Ejemplo 1: Análisis de propagación viral
// ===================================================================
void ejemploAnalisisViral(SocialNetwork* net) {
    printf("\n📱 EJEMPLO: Análisis de Propagación Viral\n");
    printf("==========================================\n");

    // Encontrar el influencer principal
    List* topInfluencers = getTopInfluencers(net, 1);
    if (!topInfluencers || topInfluencers->size == 0) return;

    User* influencer = (User*)listGet(topInfluencers, 0);
    printf("Influencer seleccionado: %s\n", influencer->username);

    // Simular publicación viral
    Post* viralPost = (Post*)malloc(sizeof(Post));
    viralPost->postId = 9999;
    viralPost->authorId = influencer->userId;
    viralPost->content = strdup("¡Contenido viral!");
    viralPost->timestamp = time(NULL);
    viralPost->likes = influencer->followersCount * 3;
    viralPost->shares = influencer->followersCount;
    viralPost->viralityScore = 0.95;

    // Calcular alcance potencial
    double alcance = calculateInfluenceSpread(net, influencer->userId);
    printf("Alcance potencial: %.0f usuarios\n", alcance);

    // Analizar velocidad de propagación por niveles
    SearchResult* bfsResult = bfs(net->userNetwork, net->maxUsers, influencer->userId - 1);

    int niveles[10] = {0};  // Usuarios por nivel de distancia
    for (int i = 0; i < net->maxUsers; i++) {
        if (bfsResult->visited[i] && bfsResult->distances[i] < 10) {
            niveles[bfsResult->distances[i]]++;
        }
    }

    printf("\nPropagación por niveles:\n");
    for (int i = 0; i < 5; i++) {
        printf("  Nivel %d: %d usuarios\n", i, niveles[i]);
    }

    freeSearchResult(bfsResult);
    freeList(topInfluencers);
    free(viralPost->content);
    free(viralPost);
}

// ===================================================================
// Ejemplo 2: Detección de cámaras de eco
// ===================================================================
void ejemploCamarasEco(SocialNetwork* net) {
    printf("\n🔊 EJEMPLO: Detección de Cámaras de Eco\n");
    printf("=========================================\n");

    // Encontrar comunidades muy cohesionadas
    List* communities = findCommunities(net, 5);

    ListNode* node = communities->head;
    int camarasEco = 0;

    while (node) {
        Community* comm = (Community*)node->data;

        // Una cámara de eco tiene alta cohesión (>80%) y poco contacto externo
        if (comm->cohesionScore > 0.8) {
            camarasEco++;
            printf("\n⚠️  Posible cámara de eco detectada:\n");
            printf("  - Tamaño: %d miembros\n", comm->members->size);
            printf("  - Cohesión interna: %.1f%%\n", comm->cohesionScore * 100);

            // Calcular conexiones externas
            int conexionesExternas = 0;
            int conexionesInternas = 0;

            ListNode* member1 = comm->members->head;
            while (member1) {
                int* userId = (int*)member1->data;
                User* user = findUserById(net, *userId);

                if (user) {
                    ListNode* conn = user->connections->head;
                    while (conn) {
                        int* connId = (int*)conn->data;

                        // Verificar si la conexión es interna o externa
                        bool esInterna = false;
                        ListNode* member2 = comm->members->head;
                        while (member2) {
                            int* memberId = (int*)member2->data;
                            if (*connId == *memberId) {
                                esInterna = true;
                                conexionesInternas++;
                                break;
                            }
                            member2 = member2->next;
                        }

                        if (!esInterna) conexionesExternas++;
                        conn = conn->next;
                    }
                }
                member1 = member1->next;
            }

            double ratioAislamiento = (double)conexionesInternas /
                                     (conexionesInternas + conexionesExternas);
            printf("  - Ratio de aislamiento: %.1f%%\n", ratioAislamiento * 100);
        }

        node = node->next;
    }

    printf("\nTotal de cámaras de eco detectadas: %d\n", camarasEco);
    freeList(communities);
}

// ===================================================================
// Ejemplo 3: Identificación de puentes sociales
// ===================================================================
void ejemploPuentesSociales(SocialNetwork* net) {
    printf("\n🌉 EJEMPLO: Identificación de Puentes Sociales\n");
    printf("==============================================\n");

    // Un puente social conecta diferentes comunidades
    typedef struct {
        User* user;
        int comunidadesConectadas;
        double centralidad;
    } PuenteSocial;

    PuenteSocial* puentes = (PuenteSocial*)calloc(net->numUsers, sizeof(PuenteSocial));

    // Detectar comunidades
    List* communities = findCommunities(net, 3);

    // Para cada usuario, contar a cuántas comunidades diferentes está conectado
    for (int userId = 1; userId <= net->numUsers; userId++) {
        User* user = findUserById(net, userId);
        if (!user) continue;

        puentes[userId - 1].user = user;

        // Bitmap para marcar comunidades conectadas
        bool* conectadoAComunidad = (bool*)calloc(communities->size, sizeof(bool));

        ListNode* connNode = user->connections->head;
        while (connNode) {
            int* connId = (int*)connNode->data;

            // Encontrar a qué comunidad pertenece esta conexión
            ListNode* commNode = communities->head;
            int commIndex = 0;
            while (commNode) {
                Community* comm = (Community*)commNode->data;
                ListNode* memberNode = comm->members->head;

                while (memberNode) {
                    int* memberId = (int*)memberNode->data;
                    if (*memberId == *connId) {
                        conectadoAComunidad[commIndex] = true;
                        break;
                    }
                    memberNode = memberNode->next;
                }

                commNode = commNode->next;
                commIndex++;
            }

            connNode = connNode->next;
        }

        // Contar comunidades conectadas
        for (int i = 0; i < communities->size; i++) {
            if (conectadoAComunidad[i]) {
                puentes[userId - 1].comunidadesConectadas++;
            }
        }

        free(conectadoAComunidad);
    }

    // Mostrar los principales puentes sociales
    printf("Principales puentes sociales:\n");
    for (int i = 0; i < net->numUsers && i < 5; i++) {
        int maxPuentes = 0;
        int maxIndex = -1;

        for (int j = 0; j < net->numUsers; j++) {
            if (puentes[j].comunidadesConectadas > maxPuentes) {
                maxPuentes = puentes[j].comunidadesConectadas;
                maxIndex = j;
            }
        }

        if (maxIndex >= 0 && puentes[maxIndex].user) {
            printf("%d. %s - Conecta %d comunidades\n",
                   i + 1,
                   puentes[maxIndex].user->username,
                   puentes[maxIndex].comunidadesConectadas);
            puentes[maxIndex].comunidadesConectadas = -1; // Marcar como procesado
        }
    }

    free(puentes);
    freeList(communities);
}

// ===================================================================
// Ejemplo 4: Análisis de tendencias
// ===================================================================
void ejemploAnalisisTendencias(SocialNetwork* net) {
    printf("\n📈 EJEMPLO: Análisis de Tendencias\n");
    printf("===================================\n");

    // Simular hashtags/temas trending
    typedef struct {
        char* tema;
        int menciones;
        double velocidadCrecimiento;
        List* usuariosActivos;
    } Tendencia;

    Tendencia tendencias[5] = {
        {"#IA", 0, 0.0, createList()},
        {"#Blockchain", 0, 0.0, createList()},
        {"#CloudComputing", 0, 0.0, createList()},
        {"#CyberSecurity", 0, 0.0, createList()},
        {"#DataScience", 0, 0.0, createList()}
    };

    // Simular actividad en temas
    for (int i = 1; i <= net->numUsers && i <= 30; i++) {
        User* user = findUserById(net, i);
        if (!user) continue;

        // Asignar tema basado en conexiones e influencia
        int temaIndex = (i - 1) % 5;
        tendencias[temaIndex].menciones += 1 + (int)(user->influenceScore / 20);

        int* userId = (int*)malloc(sizeof(int));
        *userId = i;
        listAppend(tendencias[temaIndex].usuariosActivos, userId);

        // Velocidad basada en influencia
        tendencias[temaIndex].velocidadCrecimiento += user->influenceScore / 100.0;
    }

    // Ordenar por menciones
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (tendencias[j].menciones < tendencias[j + 1].menciones) {
                Tendencia temp = tendencias[j];
                tendencias[j] = tendencias[j + 1];
                tendencias[j + 1] = temp;
            }
        }
    }

    // Mostrar trending topics
    printf("🔥 Trending Topics:\n\n");
    for (int i = 0; i < 5; i++) {
        printf("%d. %s\n", i + 1, tendencias[i].tema);
        printf("   📊 Menciones: %d\n", tendencias[i].menciones);
        printf("   🚀 Velocidad: %.2fx\n", tendencias[i].velocidadCrecimiento);
        printf("   👥 Usuarios activos: %d\n", tendencias[i].usuariosActivos->size);

        // Influencer principal del tema
        double maxInfluence = 0.0;
        User* topicInfluencer = NULL;
        ListNode* node = tendencias[i].usuariosActivos->head;
        while (node) {
            int* userId = (int*)node->data;
            User* user = findUserById(net, *userId);
            if (user && user->influenceScore > maxInfluence) {
                maxInfluence = user->influenceScore;
                topicInfluencer = user;
            }
            node = node->next;
        }

        if (topicInfluencer) {
            printf("   ⭐ Líder de opinión: %s\n", topicInfluencer->username);
        }
        printf("\n");

        freeList(tendencias[i].usuariosActivos);
    }
}

// ===================================================================
// Función principal con todos los ejemplos
// ===================================================================
void ejecutarEjemplosAvanzados() {
    printf("\n🚀 EJEMPLOS AVANZADOS DE ANÁLISIS DE REDES SOCIALES\n");
    printf("==================================================\n");

    // Crear red de prueba
    SocialNetwork* net = createSocialNetwork(200);

    // Generar red más compleja
    generateRandomNetwork(net, 100, 0.05);  // 100 usuarios, 5% probabilidad conexión

    // Crear algunos influencers
    for (int i = 1; i <= 5; i++) {
        User* user = findUserById(net, i);
        if (user) {
            user->isVerified = true;
            user->influenceScore = 70.0 + rand() % 30;

            // Agregar más conexiones a influencers
            for (int j = 20; j <= 40; j++) {
                if (rand() % 100 < 60) {
                    addConnection(net, i, j, "follower", 0.7);
                }
            }
        }
    }

    // Actualizar scores
    for (int i = 1; i <= net->numUsers; i++) {
        updateInfluenceScore(net, i);
    }

    // Ejecutar ejemplos
    ejemploAnalisisViral(net);
    printf("\n" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "\n");

    ejemploCamarasEco(net);
    printf("\n" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "\n");

    ejemploPuentesSociales(net);
    printf("\n" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "\n");

    ejemploAnalisisTendencias(net);

    // Limpiar
    destroySocialNetwork(net);

    printf("\n✅ Ejemplos completados\n");
}

// Para compilar como programa independiente
#ifdef STANDALONE_EXAMPLES
int main() {
    srand(time(NULL));
    ejecutarEjemplosAvanzados();
    return 0;
}
#endif