//
// Created by administrador on 5/27/25.
//

#ifndef SOCIAL_NETWORK_H
#define SOCIAL_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Incluir estructuras de datos existentes
#include "../algoritmos/dfs_bfs.h"
#include "../algoritmos/dijkstra.h"
#include "../algoritmos/cycle_detection.h"
#include "../estructura_datos/hash_map.h"
#include "../graph/graph.h"

// ===================================================================
// Estructuras principales del Sistema de Redes Sociales
// ===================================================================

// Lista enlazada genérica
typedef struct ListNode {
    void* data;
    struct ListNode* next;
} ListNode;

typedef struct {
    ListNode* head;
    ListNode* tail;
    int size;
} List;

// Usuario en la red social
typedef struct {
    int userId;
    char* username;
    char* fullName;
    List* connections;      // Lista de IDs de conexiones
    List* posts;           // Lista de publicaciones
    double influenceScore;  // Puntuación de influencia (0.0 - 100.0)
    int followersCount;
    int followingCount;
    time_t joinDate;
    bool isActive;
    bool isVerified;
} User;

// Conexión entre usuarios (arista con peso)
typedef struct {
    int fromUserId;
    int toUserId;
    double connectionStrength;  // 0.0 - 1.0 (fuerza de la conexión)
    time_t connectionDate;
    char* connectionType;      // "friend", "follower", "family", "colleague"
} Connection;

// Publicación de usuario
typedef struct {
    int postId;
    int authorId;
    char* content;
    time_t timestamp;
    int likes;
    int shares;
    List* comments;
    double viralityScore;
} Post;

// Comunidad detectada
typedef struct {
    int communityId;
    List* members;         // Lista de IDs de usuarios
    double cohesionScore;  // Qué tan unida está la comunidad
    char* description;
    User* influencer;      // Usuario más influyente de la comunidad
} Community;

// Red social completa
typedef struct {
    int** userNetwork;         // Matriz de adyacencia (grafo)
    HashMap* userIndex;        // Índice rápido username -> User*
    HashMap* userIdIndex;      // Índice rápido userId -> User*
    List* communities;         // Comunidades detectadas
    int numUsers;
    int maxUsers;
    int nextUserId;
    double avgConnectionsPerUser;
} SocialNetwork;

// Resultado de análisis de influencia
typedef struct {
    int userId;
    double directInfluence;    // Influencia directa (seguidores)
    double indirectInfluence;  // Influencia indirecta (alcance)
    double engagementRate;     // Tasa de interacción
    double totalInfluence;     // Puntuación total
    List* influencePath;       // Cadena de influencia
} InfluenceAnalysis;

// ===================================================================
// Prototipos de funciones - Gestión de listas
// ===================================================================

List* createList();
void listAppend(List* list, void* data);
void* listGet(List* list, int index);
bool listContains(List* list, void* data, bool (*comparator)(void*, void*));
void freeList(List* list);

// ===================================================================
// Prototipos de funciones - Sistema de Red Social
// ===================================================================

// Creación y destrucción
SocialNetwork* createSocialNetwork(int maxUsers);
void destroySocialNetwork(SocialNetwork* network);

// Gestión de usuarios
User* createUser(const char* username, const char* fullName);
int addUser(SocialNetwork* net, const char* username, const char* fullName);
bool removeUser(SocialNetwork* net, int userId);
User* findUserByUsername(SocialNetwork* net, const char* username);
User* findUserById(SocialNetwork* net, int userId);
void updateInfluenceScore(SocialNetwork* net, int userId);

// Gestión de conexiones
bool addConnection(SocialNetwork* net, int userId1, int userId2,
                  const char* connectionType, double strength);
bool removeConnection(SocialNetwork* net, int userId1, int userId2);
bool areConnected(SocialNetwork* net, int userId1, int userId2);
double getConnectionStrength(SocialNetwork* net, int userId1, int userId2);
List* getConnections(SocialNetwork* net, int userId);
List* getMutualConnections(SocialNetwork* net, int userId1, int userId2);

// ===================================================================
// Análisis de red - Funciones principales
// ===================================================================

// Grados de separación (usando BFS)
int calculateSeparationDegree(SocialNetwork* net, int userId1, int userId2);
double calculateAverageSeparation(SocialNetwork* net);

// Detección de comunidades (usando DFS)
List* findCommunities(SocialNetwork* net, int minSize);
Community* findUserCommunity(SocialNetwork* net, int userId);
double calculateCommunityCohesion(SocialNetwork* net, Community* community);

// Detección de círculos de amigos (usando detección de ciclos)
List* detectFriendCircles(SocialNetwork* net, int userId);
List* findCliques(SocialNetwork* net, int minSize);
bool isInFriendCircle(SocialNetwork* net, int userId1, int userId2);

// Análisis de influencia (usando Dijkstra modificado)
double calculateInfluenceSpread(SocialNetwork* net, int userId);
InfluenceAnalysis* analyzeUserInfluence(SocialNetwork* net, int userId);
List* findInfluencePath(SocialNetwork* net, int fromUser, int toUser);
List* getTopInfluencers(SocialNetwork* net, int topN);

// ===================================================================
// Funciones de análisis adicionales
// ===================================================================

// Métricas de red
double calculateNetworkDensity(SocialNetwork* net);
double calculateClusteringCoefficient(SocialNetwork* net, int userId);
double calculateGlobalClusteringCoefficient(SocialNetwork* net);
int* calculateDegreeDistribution(SocialNetwork* net, int* maxDegree);

// Recomendaciones
List* recommendConnections(SocialNetwork* net, int userId, int maxRecommendations);
List* recommendByMutualConnections(SocialNetwork* net, int userId);
List* recommendByCommunity(SocialNetwork* net, int userId);

// Análisis de centralidad
double calculateDegreeCentrality(SocialNetwork* net, int userId);
double calculateBetweennessCentrality(SocialNetwork* net, int userId);
double calculateClosenessCentrality(SocialNetwork* net, int userId);

// ===================================================================
// Funciones de visualización y exportación
// ===================================================================

void printUserInfo(User* user);
void printNetworkStatistics(SocialNetwork* net);
void printCommunityInfo(Community* community);
void visualizeNetworkASCII(SocialNetwork* net);
void exportNetworkToGraphviz(SocialNetwork* net, const char* filename);
void exportCommunitiesToCSV(SocialNetwork* net, const char* filename);

// ===================================================================
// Funciones de utilidad
// ===================================================================

void generateRandomNetwork(SocialNetwork* net, int numUsers, double connectionProbability);
void simulateViralSpread(SocialNetwork* net, int startUserId, double spreadProbability);
List* findShortestPath(SocialNetwork* net, int userId1, int userId2);
bool detectFakeAccounts(SocialNetwork* net, double threshold);

// Demo y pruebas
void runSocialNetworkDemo();

#endif // SOCIAL_NETWORK_H