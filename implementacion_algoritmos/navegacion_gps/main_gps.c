#include "gps_system.h"

int main() {
    printf("🚀 SISTEMA DE NAVEGACIÓN GPS AVANZADO\n");
    printf("=====================================\n");

    int choice;
    printf("¿Desea ejecutar la demo automática? (1=Sí, 0=Menú interactivo): ");
    scanf("%d", &choice);

    if (choice == 1) {
        runGPSDemo();
    } else {
        // Modo interactivo
        NavigationSystem* gps = createNavigationSystem(100);

        int option;
        do {
            showMenu();
            scanf("%d", &option);

            switch (option) {
                case 1: {
                    char name[MAX_NAME_LENGTH], region[MAX_NAME_LENGTH];
                    double lat, lon;
                    int population;
                    printf("Nombre de la ciudad: ");
                    scanf("%s", name);
                    printf("Latitud: ");
                    scanf("%lf", &lat);
                    printf("Longitud: ");
                    scanf("%lf", &lon);
                    printf("Población: ");
                    scanf("%d", &population);
                    printf("Región: ");
                    scanf("%s", region);
                    addCity(gps, name, lat, lon, population, region);
                    break;
                }
                case 2: {
                    char fromCity[MAX_NAME_LENGTH], toCity[MAX_NAME_LENGTH], roadType[20];
                    double distance, travelTime, toll;
                    int speedLimit;
                    printf("Ciudad origen: ");
                    scanf("%s", fromCity);
                    printf("Ciudad destino: ");
                    scanf("%s", toCity);
                    printf("Distancia (km): ");
                    scanf("%lf", &distance);
                    printf("Tiempo de viaje (min): ");
                    scanf("%lf", &travelTime);
                    printf("Peaje ($): ");
                    scanf("%lf", &toll);
                    printf("Tipo de carretera: ");
                    scanf("%s", roadType);
                    printf("Límite de velocidad: ");
                    scanf("%d", &speedLimit);
                    addRoad(gps, fromCity, toCity, distance, travelTime, toll, roadType, speedLimit);
                    break;
                }
                case 3:
                    listCities(gps);
                    break;
                case 4: {
                    char from[MAX_NAME_LENGTH], to[MAX_NAME_LENGTH];
                    printf("Ciudad origen: ");
                    scanf("%s", from);
                    printf("Ciudad destino: ");
                    scanf("%s", to);
                    Route* route = findShortestPath(gps, from, to);
                    if (route) {
                        printRoute(gps, route);
                        freeRoute(route);
                    }
                    break;
                }
                case 5: {
                    char from[MAX_NAME_LENGTH], to[MAX_NAME_LENGTH];
                    printf("Ciudad origen: ");
                    scanf("%s", from);
                    printf("Ciudad destino: ");
                    scanf("%s", to);
                    Route* route = findFastestPath(gps, from, to);
                    if (route) {
                        printRoute(gps, route);
                        freeRoute(route);
                    }
                    break;
                }
                case 6: {
                    char from[MAX_NAME_LENGTH], to[MAX_NAME_LENGTH];
                    printf("Ciudad origen: ");
                    scanf("%s", from);
                    printf("Ciudad destino: ");
                    scanf("%s", to);
                    Route* route = findCheapestPath(gps, from, to);
                    if (route) {
                        printRoute(gps, route);
                        freeRoute(route);
                    }
                    break;
                }
                case 7: {
                    char from[MAX_NAME_LENGTH], to[MAX_NAME_LENGTH];
                    printf("Ciudad origen: ");
                    scanf("%s", from);
                    printf("Ciudad destino: ");
                    scanf("%s", to);
                    bool reachable = isReachable(gps, from, to);
                    printf("¿%s → %s? %s\n", from, to, reachable ? "✅ SÍ" : "❌ NO");
                    break;
                }
                case 8: {
                    char from[MAX_NAME_LENGTH], to[MAX_NAME_LENGTH];
                    double factor;
                    printf("Ciudad origen: ");
                    scanf("%s", from);
                    printf("Ciudad destino: ");
                    scanf("%s", to);
                    printf("Factor de tráfico (1.0 = normal, 2.0 = doble tiempo): ");
                    scanf("%lf", &factor);
                    updateTrafficConditions(gps, from, to, factor);
                    break;
                }
                case 9: {
                    bool hasCycles = detectRouteLoops(gps);
                    printf("¿Hay ciclos en la red? %s\n", hasCycles ? "✅ SÍ" : "❌ NO");
                    break;
                }
                case 10:
                    generateNetworkStatistics(gps);
                    break;
                case 11:
                    printf("⚠️  Funcionalidad de guardado no implementada aún\n");
                    break;
                case 12:
                    printf("⚠️  Funcionalidad de carga no implementada aún\n");
                    break;
                case 13:
                    clearRouteCache(gps);
                    break;
                case 14:
                    printf("⚠️  Exportación a SVG no implementada aún\n");
                    break;
                case 15:
                    gps->debugMode = !gps->debugMode;
                    printf("Debug mode: %s\n", gps->debugMode ? "ON" : "OFF");
                    break;
                case 0:
                    printf("¡Gracias por usar el Sistema GPS! 🚗\n");
                    break;
                default:
                    printf("❌ Opción no válida\n");
            }
        } while (option != 0);

        destroyNavigationSystem(gps);
    }

    return 0;
}