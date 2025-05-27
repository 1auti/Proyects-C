#include "social_network.h"
#include <math.h>
#include <float.h>

// ===================================================================
// Implementación de Lista enlazada
// ===================================================================

List* createList() {
    List* list = (List*)malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void listAppend(List* list, void* data) {
    if (!list) return;
    
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->data = data;
    node->next = NULL;
    
    if (!list->head) {
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

void* listGet(List* list, int index) {
    if (!list || index < 0 || index >= list->size) return NULL;
    
    ListNode* current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

bool listContains(List* list, void* data, bool (*comparator)(void*, void*)) {
    if (!list) return false;
    
    ListNode* current = list->head;
    while (current) {
        if (comparator(current->data, data)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void freeList(List* list) {
    if (!list) return;
    
    ListNode* current = list->head;
    while (current) {
        ListNode* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

// ===================================================================
// Creación y destrucción del sistema
// ===================================================================

SocialNetwork* createSocialNetwork(int maxUsers) {
    SocialNetwork* net = (SocialNetwork*)malloc(sizeof(SocialNetwork));
    if (!net) return NULL;
    
    net->maxUsers = maxUsers;
    net->numUsers = 0;
    net->nextUserId = 1;
    net->avgConnectionsPerUser = 0.0;
    
    // Crear matriz de adyacencia
    net->userNetwork = (int**)malloc(maxUsers * sizeof(int*));
    for (int i = 0; i < maxUsers; i++) {
        net->userNetwork[i] = (int*)calloc(maxUsers, sizeof(int));
    }
    
    // Crear índices hash
    net->userIndex = createHashMap(maxUsers);
    net->userIdIndex = createHashMap(maxUsers);
    net->communities = createList();
    
    return net;
}

void destroySocialNetwork(SocialNetwork* network) {
    if (!network) return;
    
    // Liberar matriz de adyacencia
    for (int i = 0; i < network->maxUsers; i++) {
        free(network->userNetwork[i]);
    }
    free(network->userNetwork);
    
    // Liberar usuarios
    int numKeys;
    char** keys = hashMapGetKeys(network->userIndex, &numKeys);
    for (int i = 0; i < numKeys; i++) {
        User* user = (User*)hashMapGet(network->userIndex, keys[i]);
        if (user) {
            free(user->username);
            free(user->fullName);
            freeList(user->connections);
            freeList(user->posts);
            free(user);
        }
        free(keys[i]);
    }
    free(keys);
    
    // Liberar estructuras
    destroyHashMap(network->userIndex);
    destroyHashMap(network->userIdIndex);
    freeList(network->communities);
    free(network);
}

// ===================================================================
// Gestión de usuarios
// ===================================================================

User* createUser(const char* username, const char* fullName) {
    User* user = (User*)malloc(sizeof(User));
    if (!user) return NULL;
    
    user->userId = 0;  // Se asignará al agregar a la red
    user->username = strdup(username);
    user->fullName = strdup(fullName);
    user->connections = createList();
    user->posts = createList();
    user->influenceScore = 0.0;
    user->followersCount = 0;
    user->followingCount = 0;
    user->joinDate = time(NULL);
    user->isActive = true;
    user->isVerified = false;
    
    return user;
}

int addUser(SocialNetwork* net, const char* username, const char* fullName) {
    if (!net || !username || net->numUsers >= net->maxUsers) return -1;
    
    // Verificar si el usuario ya existe
    if (hashMapContains(net->userIndex, username)) {
        return -1;
    }
    
    User* user = createUser(username, fullName);
    if (!user) return -1;
    
    user->userId = net->nextUserId++;
    
    // Agregar a los índices
    hashMapPut(net->userIndex, username, user);
    
    char idStr[20];
    sprintf(idStr, "%d", user->userId);
    hashMapPut(net->userIdIndex, idStr, user);
    
    net->numUsers++;
    
    return user->userId;
}

User* findUserByUsername(SocialNetwork* net, const char* username) {
    if (!net || !username) return NULL;
    return (User*)hashMapGet(net->userIndex, username);
}

User* findUserById(SocialNetwork* net, int userId) {
    if (!net || userId <= 0) return NULL;
    
    char idStr[20];
    sprintf(idStr, "%d", userId);
    return (User*)hashMapGet(net->userIdIndex, idStr);
}

void updateInfluenceScore(SocialNetwork* net, int userId) {
    User* user = findUserById(net, userId);
    if (!user) return;
    
    // Calcular score basado en seguidores, verificación y actividad
    double baseScore = user->followersCount * 0.5;
    double verifiedBonus = user->isVerified ? 20.0 : 0.0;
    double activityBonus = user->posts->size * 0.1;
    
    // Factor de engagement (likes/shares por post)
    double engagementScore = 0.0;
    ListNode* postNode = user->posts->head;
    while (postNode) {
        Post* post = (Post*)postNode->data;
        engagementScore += (post->likes * 0.01 + post->shares * 0.05);
        postNode = postNode->next;
    }
    
    user->influenceScore = fmin(100.0, baseScore + verifiedBonus + 
                                      activityBonus + engagementScore);
}

// ===================================================================
// Gestión de conexiones
// ===================================================================

bool addConnection(SocialNetwork* net, int userId1, int userId2, 
                  const char* connectionType, double strength) {
    if (!net || userId1 <= 0 || userId2 <= 0 || userId1 == userId2) {
        return false;
    }
    
    User* user1 = findUserById(net, userId1);
    User* user2 = findUserById(net, userId2);
    
    if (!user1 || !user2) return false;
    
    // Actualizar matriz de adyacencia (peso = strength * 100)
    int weight = (int)(strength * 100);
    net->userNetwork[userId1 - 1][userId2 - 1] = weight;
    net->userNetwork[userId2 - 1][userId1 - 1] = weight;
    
    // Agregar a listas de conexiones
    int* id1 = (int*)malloc(sizeof(int));
    int* id2 = (int*)malloc(sizeof(int));
    *id1 = userId1;
    *id2 = userId2;
    
    listAppend(user1->connections, id2);
    listAppend(user2->connections, id1);
    
    user1->followingCount++;
    user2->followersCount++;
    
    // Actualizar scores de influencia
    updateInfluenceScore(net, userId1);
    updateInfluenceScore(net, userId2);
    
    return true;
}

bool areConnected(SocialNetwork* net, int userId1, int userId2) {
    if (!net || userId1 <= 0 || userId2 <= 0) return false;
    return net->userNetwork[userId1 - 1][userId2 - 1] > 0;
}

double getConnectionStrength(SocialNetwork* net, int userId1, int userId2) {
    if (!net || userId1 <= 0 || userId2 <= 0) return 0.0;
    return net->userNetwork[userId1 - 1][userId2 - 1] / 100.0;
}

// ===================================================================
// Análisis de red - Grados de separación
// ===================================================================

int calculateSeparationDegree(SocialNetwork* net, int userId1, int userId2) {
    if (!net || userId1 <= 0 || userId2 <= 0) return -1;
    if (userId1 == userId2) return 0;
    
    // Usar BFS para encontrar el camino más corto
    SearchResult* result = bfs(net->userNetwork, net->maxUsers, userId1 - 1);
    
    if (!result) return -1;
    
    int separation = -1;
    if (result->visited[userId2 - 1]) {
        separation = result->distances[userId2 - 1];
    }
    
    freeSearchResult(result);
    return separation;
}

double calculateAverageSeparation(SocialNetwork* net) {
    if (!net || net->numUsers < 2) return 0.0;
    
    double totalSeparation = 0.0;
    int validPairs = 0;
    
    // Calcular separación promedio entre todos los pares de usuarios
    for (int i = 1; i <= net->numUsers; i++) {
        for (int j = i + 1; j <= net->numUsers; j++) {
            int separation = calculateSeparationDegree(net, i, j);
            if (separation > 0) {
                totalSeparation += separation;
                validPairs++;
            }
        }
    }
    
    return validPairs > 0 ? totalSeparation / validPairs : 0.0;
}

// ===================================================================
// Detección de comunidades usando DFS
// ===================================================================

List* findCommunities(SocialNetwork* net, int minSize) {
    if (!net) return NULL;
    
    List* communities = createList();
    bool* visited = (bool*)calloc(net->maxUsers, sizeof(bool));
    int communityId = 1;
    
    // Encontrar componentes conexos
    for (int i = 0; i < net->numUsers; i++) {
        if (!visited[i]) {
            SearchResult* dfsResult = dfs(net->userNetwork, net->maxUsers, i);
            
            if (dfsResult && dfsResult->visitCount >= minSize) {
                Community* community = (Community*)malloc(sizeof(Community));
                community->communityId = communityId++;
                community->members = createList();
                community->cohesionScore = 0.0;
                
                // Agregar miembros de la comunidad
                for (int j = 0; j < dfsResult->visitCount; j++) {
                    int* userId = (int*)malloc(sizeof(int));
                    *userId = dfsResult->visitOrder[j] + 1;
                    listAppend(community->members, userId);
                    visited[dfsResult->visitOrder[j]] = true;
                }
                
                // Calcular cohesión
                community->cohesionScore = calculateCommunityCohesion(net, community);
                
                // Encontrar influencer principal
                double maxInfluence = 0.0;
                ListNode* node = community->members->head;
                while (node) {
                    int* userId = (int*)node->data;
                    User* user = findUserById(net, *userId);
                    if (user && user->influenceScore > maxInfluence) {
                        maxInfluence = user->influenceScore;
                        community->influencer = user;
                    }
                    node = node->next;
                }
                
                listAppend(communities, community);
            }
            
            if (dfsResult) freeSearchResult(dfsResult);
        }
    }
    
    free(visited);
    return communities;
}

double calculateCommunityCohesion(SocialNetwork* net, Community* community) {
    if (!net || !community || community->members->size < 2) return 0.0;
    
    int internalConnections = 0;
    int possibleConnections = 0;
    
    ListNode* node1 = community->members->head;
    while (node1) {
        int* userId1 = (int*)node1->data;
        ListNode* node2 = node1->next;
        
        while (node2) {
            int* userId2 = (int*)node2->data;
            possibleConnections++;
            
            if (areConnected(net, *userId1, *userId2)) {
                internalConnections++;
            }
            
            node2 = node2->next;
        }
        node1 = node1->next;
    }
    
    return possibleConnections > 0 ? 
           (double)internalConnections / possibleConnections : 0.0;
}

// ===================================================================
// Detección de círculos de amigos
// ===================================================================

List* detectFriendCircles(SocialNetwork* net, int userId) {
    if (!net || userId <= 0) return NULL;
    
    List* circles = createList();
    User* user = findUserById(net, userId);
    if (!user) return circles;
    
    // Crear subgrafo con amigos del usuario
    int friendCount = user->connections->size;
    if (friendCount < 2) return circles;
    
    // Detectar ciclos en el subgrafo de amigos
    bool hasCycles = hasCycleUndirected(net->userNetwork, net->maxUsers);
    
    if (hasCycles) {
        // Encontrar cliques usando búsqueda exhaustiva
        // (Simplificado - en producción usar algoritmo de Bron-Kerbosch)
        ListNode* node1 = user->connections->head;
        while (node1) {
            int* friend1 = (int*)node1->data;
            ListNode* node2 = node1->next;
            
            while (node2) {
                int* friend2 = (int*)node2->data;
                
                if (areConnected(net, *friend1, *friend2)) {
                    // Formar círculo de 3 personas
                    List* circle = createList();
                    
                    int* id1 = (int*)malloc(sizeof(int));
                    int* id2 = (int*)malloc(sizeof(int));
                    int* id3 = (int*)malloc(sizeof(int));
                    *id1 = userId;
                    *id2 = *friend1;
                    *id3 = *friend2;
                    
                    listAppend(circle, id1);
                    listAppend(circle, id2);
                    listAppend(circle, id3);
                    
                    listAppend(circles, circle);
                }
                
                node2 = node2->next;
            }
            node1 = node1->next;
        }
    }
    
    return circles;
}

// ===================================================================
// Análisis de influencia usando Dijkstra modificado
// ===================================================================

double calculateInfluenceSpread(SocialNetwork* net, int userId) {
    if (!net || userId <= 0) return 0.0;
    
    User* user = findUserById(net, userId);
    if (!user) return 0.0;
    
    // Usar Dijkstra para calcular alcance ponderado
    int* distances = dijkstraAllPaths(net->userNetwork, net->maxUsers, userId - 1);
    
    double totalInfluence = user->influenceScore;
    int reachableUsers = 0;
    
    for (int i = 0; i < net->maxUsers; i++) {
        if (i != userId - 1 && distances[i] != INF) {
            reachableUsers++;
            // Influencia decrece con la distancia
            double distanceFactor = 1.0 / (1.0 + distances[i]);
            totalInfluence += user->influenceScore * distanceFactor;
        }
    }
    
    free(distances);
    
    // Factor de alcance
    double reachFactor = (double)reachableUsers / (net->numUsers - 1);
    return totalInfluence * reachFactor;
}

InfluenceAnalysis* analyzeUserInfluence(SocialNetwork* net, int userId) {
    if (!net || userId <= 0) return NULL;
    
    User* user = findUserById(net, userId);
    if (!user) return NULL;
    
    InfluenceAnalysis* analysis = (InfluenceAnalysis*)malloc(sizeof(InfluenceAnalysis));
    analysis->userId = userId;
    analysis->directInfluence = user->influenceScore;
    analysis->indirectInfluence = calculateInfluenceSpread(net, userId) - user->influenceScore;
    analysis->engagementRate = 0.0;
    
    // Calcular engagement rate
    if (user->posts->size > 0) {
        double totalEngagement = 0.0;
        ListNode* node = user->posts->head;
        while (node) {
            Post* post = (Post*)node->data;
            totalEngagement += (double)(post->likes + post->shares) / user->followersCount;
            node = node->next;
        }
        analysis->engagementRate = totalEngagement / user->posts->size;
    }
    
    analysis->totalInfluence = analysis->directInfluence + 
                              analysis->indirectInfluence * 0.3 + 
                              analysis->engagementRate * 20.0;
    
    analysis->influencePath = createList();
    
    return analysis;
}

List* getTopInfluencers(SocialNetwork* net, int topN) {
    if (!net || topN <= 0) return NULL;
    
    List* influencers = createList();
    
    // Crear array de todos los usuarios con sus scores
    typedef struct {
        User* user;
        double score;
    } UserScore;
    
    UserScore* scores = (UserScore*)malloc(net->numUsers * sizeof(UserScore));
    int count = 0;
    
    // Calcular scores de influencia para todos
    for (int i = 1; i <= net->numUsers; i++) {
        User* user = findUserById(net, i);
        if (user) {
            scores[count].user = user;
            scores[count].score = calculateInfluenceSpread(net, i);
            count++;
        }
    }
    
    // Ordenar por score (bubble sort simple)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                UserScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
    
    // Tomar los top N
    for (int i = 0; i < topN && i < count; i++) {
        listAppend(influencers, scores[i].user);
    }
    
    free(scores);
    return influencers;
}

// ===================================================================
// Recomendaciones de conexiones
// ===================================================================

List* recommendConnections(SocialNetwork* net, int userId, int maxRecommendations) {
    if (!net || userId <= 0) return NULL;
    
    List* recommendations = createList();
    User* user = findUserById(net, userId);
    if (!user) return recommendations;
    
    // Array para contar amigos en común
    int* mutualCount = (int*)calloc(net->maxUsers, sizeof(int));
    
    // Contar amigos en común con no-conexiones
    ListNode* friendNode = user->connections->head;
    while (friendNode) {
        int* friendId = (int*)friendNode->data;
        User* friend = findUserById(net, *friendId);
        
        if (friend) {
            ListNode* friendOfFriendNode = friend->connections->head;
            while (friendOfFriendNode) {
                int* fofId = (int*)friendOfFriendNode->data;
                
                // Si no es el usuario mismo y no están conectados
                if (*fofId != userId && !areConnected(net, userId, *fofId)) {
                    mutualCount[*fofId - 1]++;
                }
                
                friendOfFriendNode = friendOfFriendNode->next;
            }
        }
        
        friendNode = friendNode->next;
    }
    
    // Crear array de recomendaciones ordenado
    typedef struct {
        int userId;
        int mutuals;
    } Recommendation;
    
    Recommendation* recs = (Recommendation*)malloc(net->maxUsers * sizeof(Recommendation));
    int recCount = 0;
    
    for (int i = 0; i < net->maxUsers; i++) {
        if (mutualCount[i] > 0) {
            recs[recCount].userId = i + 1;
            recs[recCount].mutuals = mutualCount[i];
            recCount++;
        }
    }
    
    // Ordenar por número de amigos en común
    for (int i = 0; i < recCount - 1; i++) {
        for (int j = 0; j < recCount - i - 1; j++) {
            if (recs[j].mutuals < recs[j + 1].mutuals) {
                Recommendation temp = recs[j];
                recs[j] = recs[j + 1];
                recs[j + 1] = temp;
            }
        }
    }
    
    // Agregar las mejores recomendaciones
    for (int i = 0; i < maxRecommendations && i < recCount; i++) {
        User* recUser = findUserById(net, recs[i].userId);
        if (recUser) {
            listAppend(recommendations, recUser);
        }
    }
    
    free(mutualCount);
    free(recs);
    
    return recommendations;
}

// ===================================================================
// Visualización y estadísticas
// ===================================================================

void printUserInfo(User* user) {
    if (!user) return;
    
    printf("\n=== Usuario: %s ===\n", user->username);
    printf("ID: %d\n", user->userId);
    printf("Nombre completo: %s\n", user->fullName);
    printf("Seguidores: %d\n", user->followersCount);
    printf("Siguiendo: %d\n", user->followingCount);
    printf("Score de influencia: %.2f\n", user->influenceScore);
    printf("Verificado: %s\n", user->isVerified ? "Sí" : "No");
    printf("Publicaciones: %d\n", user->posts->size);
}

void printNetworkStatistics(SocialNetwork* net) {
    if (!net) return;
    
    printf("\n=== ESTADÍSTICAS DE LA RED SOCIAL ===\n");
    printf("Total de usuarios: %d\n", net->numUsers);
    
    // Calcular estadísticas
    int totalConnections = 0;
    double totalInfluence = 0.0;
    
    for (int i = 1; i <= net->numUsers; i++) {
        User* user = findUserById(net, i);
        if (user) {
            totalConnections += user->connections->size;
            totalInfluence += user->influenceScore;
        }
    }
    
    printf("Conexiones promedio por usuario: %.2f\n", 
           (double)totalConnections / net->numUsers);
    printf("Influencia promedio: %.2f\n", totalInfluence / net->numUsers);
    printf("Separación promedio: %.2f grados\n", calculateAverageSeparation(net));
    
    // Detectar comunidades
    List* communities = findCommunities(net, 3);
    printf("Comunidades detectadas (3+ miembros): %d\n", communities->size);
    
    // Top influencers
    printf("\nTop 5 Influencers:\n");
    List* topInfluencers = getTopInfluencers(net, 5);
    ListNode* node = topInfluencers->head;
    int rank = 1;
    while (node) {
        User* influencer = (User*)node->data;
        printf("%d. %s (Score: %.2f)\n", rank++, 
               influencer->username, 
               calculateInfluenceSpread(net, influencer->userId));
        node = node->next;
    }
    
    freeList(communities);
    freeList(topInfluencers);
}

// ===================================================================
// Generación de red aleatoria para pruebas
// ===================================================================

void generateRandomNetwork(SocialNetwork* net, int numUsers, double connectionProbability) {
    if (!net || numUsers <= 0) return;
    
    // Crear usuarios
    char username[50];
    char fullName[100];
    
    for (int i = 0; i < numUsers; i++) {
        sprintf(username, "user%d", i + 1);
        sprintf(fullName, "Usuario %d", i + 1);
        addUser(net, username, fullName);
    }
    
    // Crear conexiones aleatorias
    srand(time(NULL));
    
    for (int i = 1; i <= numUsers; i++) {
        for (int j = i + 1; j <= numUsers; j++) {
            if ((double)rand() / RAND_MAX < connectionProbability) {
                double strength = 0.5 + (double)rand() / RAND_MAX * 0.5;
                addConnection(net, i, j, "friend", strength);
            }
        }
    }
    
    // Verificar algunos usuarios
    for (int i = 1; i <= numUsers / 10; i++) {
        User* user = findUserById(net, rand() % numUsers + 1);
        if (user) {
            user->isVerified = true;
            updateInfluenceScore(net, user->userId);
        }
    }
}