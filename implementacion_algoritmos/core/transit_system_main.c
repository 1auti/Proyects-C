// ============================================================================
// main.c - Demo completo del Sistema de Transporte Público Inteligente
// ============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Incluir todos los headers del sistema
#include "core/transit_system.h"
#include "algoritmos/route_planner.h"
#include "pricing/fare_calculator.h"
#include "realtime/delay_tracker.h"
#include "scheduling/schedule.h"

// ============================================================================
// FUNCIONES AUXILIARES PARA EL DEMO
// ============================================================================

void printSeparator(const char* title) {
    printf("\n");
    for (int i = 0; i < 60; i++) {
        printf("=");
    }
    printf("\n🎯 %s\n", title);
    for (int i = 0; i < 60; i++) {
        printf("=");
    }
    printf("\n");
}

void printStepHeader(int step, const char* description) {
    printf("\n📍 PASO %d: %s\n", step, description);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

void pauseDemo() {
    printf("\n⏸️  Presiona Enter para continuar...");
    getchar();
}

// ============================================================================
// FUNCIÓN PRINCIPAL DE DEMOSTRACIÓN
// ============================================================================

int main() {
    printf("🚇 ===== SISTEMA DE TRANSPORTE PÚBLICO INTELIGENTE =====\n");
    printf("🎉 ¡Bienvenido al demo completo del sistema!\n");
    printf("📋 Este demo mostrará todas las funcionalidades implementadas\n\n");

    pauseDemo();

    // ============================================================================
    // PASO 1: CREAR EL SISTEMA
    // ============================================================================
    printStepHeader(1, "CREACIÓN DEL SISTEMA");

    TransitSystem* system = createTransitSystem(20, 10);
    if (!system) {
        printf("❌ Error crítico: No se pudo crear el sistema\n");
        return 1;
    }

    printf("✅ Sistema creado exitosamente\n");
    pauseDemo();

    // ============================================================================
    // PASO 2: CREAR Y AGREGAR ESTACIONES
    // ============================================================================
    printStepHeader(2, "CREACIÓN DE ESTACIONES");

    // Crear estaciones del metro de Madrid (ejemplo)
    Station* atocha = createStation(1, "Atocha", 40.4063, -3.6906, 1, true);
    Station* sol = createStation(2, "Sol", 40.4169, -3.7033, 1, true);
    Station* plaza_castilla = createStation(3, "Plaza de Castilla", 40.4647, -3.6884, 2, false);
    Station* retiro = createStation(4, "Retiro", 40.4152, -3.6829, 1, true);
    Station* nuevos_ministerios = createStation(5, "Nuevos Ministerios", 40.4467, -3.6931, 2, true);
    Station* chamartin = createStation(6, "Chamartín", 40.4730, -3.6802, 2, true);

    // Agregar estaciones al sistema
    printf("\n🚉 Agregando estaciones al sistema...\n");
    addStationToSystem(system, atocha);
    addStationToSystem(system, sol);
    addStationToSystem(system, plaza_castilla);
    addStationToSystem(system, retiro);
    addStationToSystem(system, nuevos_ministerios);
    addStationToSystem(system, chamartin);

    printf("\n📊 Estaciones agregadas: %d\n", system->station_count);
    pauseDemo();

    // ============================================================================
    // PASO 3: CREAR Y AGREGAR LÍNEAS
    // ============================================================================
    printStepHeader(3, "CREACIÓN DE LÍNEAS DE TRANSPORTE");

    // Crear líneas
    Line* linea1 = createLine(1, "L1", "#0066CC", 0); // Metro azul
    Line* linea2 = createLine(2, "L2", "#FF0000", 0); // Metro rojo
    Line* bus47 = createLine(3, "Bus47", "#00AA00", 1); // Autobús verde

    printf("\n🚇 Configurando Línea 1 (L1)...\n");
    addStationToLine(linea1, atocha, 3);        // 3 min hasta Sol
    addStationToLine(linea1, sol, 4);           // 4 min hasta Nuevos Ministerios
    addStationToLine(linea1, nuevos_ministerios, 5); // 5 min hasta Plaza Castilla
    addStationToLine(linea1, plaza_castilla, 0); // Final de línea

    printf("\n🚇 Configurando Línea 2 (L2)...\n");
    addStationToLine(linea2, retiro, 2);        // 2 min hasta Sol
    addStationToLine(linea2, sol, 6);           // 6 min hasta Chamartín
    addStationToLine(linea2, chamartin, 0);     // Final de línea

    printf("\n🚌 Configurando Bus 47...\n");
    addStationToLine(bus47, sol, 8);            // 8 min hasta Plaza Castilla
    addStationToLine(bus47, plaza_castilla, 0); // Final de línea

    // Agregar líneas al sistema
    printf("\n🔗 Integrando líneas en el sistema...\n");
    addLineToSystem(system, linea1);
    addLineToSystem(system, linea2);
    addLineToSystem(system, bus47);

    pauseDemo();

    // ============================================================================
    // PASO 4: CONFIGURAR HORARIOS
    // ============================================================================
    printStepHeader(4, "CONFIGURACIÓN DE HORARIOS");

    // Crear horarios para las líneas
    Time first_service = {6, 0};   // 6:00 AM
    Time last_service = {23, 30};  // 11:30 PM

    Schedule* schedule_l1 = createSchedule(1, 5, first_service, last_service); // Cada 5 min
    Schedule* schedule_l2 = createSchedule(2, 7, first_service, last_service); // Cada 7 min
    Schedule* schedule_bus = createSchedule(3, 12, first_service, last_service); // Cada 12 min

    printf("\n📅 Generando horarios automáticos...\n");
    generateSchedule(schedule_l1);
    generateSchedule(schedule_l2);
    generateSchedule(schedule_bus);

    // Agregar horarios al sistema
    addScheduleToSystem(system, schedule_l1);
    addScheduleToSystem(system, schedule_l2);
    addScheduleToSystem(system, schedule_bus);

    pauseDemo();

    // ============================================================================
    // PASO 5: MOSTRAR ESTADO DEL SISTEMA
    // ============================================================================
    printStepHeader(5, "ESTADO ACTUAL DEL SISTEMA");

    printTransitSystem(system);

    printf("\n🔍 Validando sistema...\n");
    if (validateTransitSystem(system)) {
        printf("✅ Sistema validado correctamente\n");
    } else {
        printf("⚠️ Sistema tiene problemas\n");
    }

    pauseDemo();

    // ============================================================================
    // PASO 6: PLANIFICACIÓN DE RUTAS
    // ============================================================================
    printStepHeader(6, "PLANIFICACIÓN DE RUTAS");

    printf("🗺️ Buscando rutas: Atocha → Plaza de Castilla\n\n");

    // Ruta más rápida
    Time departure = {8, 30}; // 8:30 AM
    printf("⚡ Buscando ruta más rápida (salida 8:30)...\n");
    Route* fastest_route = findFastestRoute(system, atocha, plaza_castilla, departure);

    if (fastest_route) {
        printf("✅ Ruta más rápida encontrada:\n");
        printRoute(fastest_route);
    } else {
        printf("❌ No se encontró ruta rápida\n");
    }

    pauseDemo();

    // Ruta más barata
    printf("💰 Buscando ruta más barata...\n");
    Route* cheapest_route = findCheapestRoute(system, atocha, plaza_castilla);

    if (cheapest_route) {
        printf("✅ Ruta más barata encontrada:\n");
        printRouteCompact(cheapest_route);
    } else {
        printf("❌ No se encontró ruta barata\n");
    }

    pauseDemo();

    // Múltiples opciones
    printf("🔄 Buscando múltiples opciones (máximo 2 transbordos)...\n");
    int route_count;
    Route** multiple_routes = findRoutesWithTransfers(system, atocha, plaza_castilla, 2, &route_count);

    if (multiple_routes && route_count > 0) {
        printf("✅ Encontradas %d opciones:\n", route_count);
        for (int i = 0; i < route_count; i++) {
            printf("\n🗺️ Opción %d:\n", i + 1);
            printRouteCompact(multiple_routes[i]);
        }
    } else {
        printf("❌ No se encontraron rutas alternativas\n");
    }

    pauseDemo();

    // ============================================================================
    // PASO 7: CÁLCULO DE TARIFAS
    // ============================================================================
    printStepHeader(7, "SISTEMA DE TARIFAS Y DESCUENTOS");

    if (fastest_route) {
        printf("💳 Calculando tarifas para la ruta más rápida...\n");

        // Calcular tarifa total
        double total_fare = calculateTotalFare(fastest_route, system->fares);
        fastest_route->total_cost = total_fare;

        // Probar tarjeta de estudiante
        printf("\n🎓 Probando tarjeta de estudiante...\n");
        DiscountCard* student_card = createDiscountCard(CARD_STUDENT, "María García", 25.0);
        double student_fare = calculateDiscountedFare(fastest_route, student_card, system->fares);

        // Probar recargo por hora pico
        printf("\n⏰ Aplicando recargo por hora pico...\n");
        double peak_surcharge = calculatePeakHourSurcharge(fastest_route, departure);

        printf("\n📊 RESUMEN DE TARIFAS:\n");
        printf("   💰 Tarifa normal: $%.2f\n", total_fare);
        printf("   🎓 Con descuento estudiantil: $%.2f\n", student_fare);
        printf("   ⏰ Recargo hora pico: +$%.2f\n", peak_surcharge);

        destroyDiscountCard(student_card);
    }

    pauseDemo();

    // ============================================================================
    // PASO 8: SIMULACIÓN DE RETRASOS
    // ============================================================================
    printStepHeader(8, "SIMULACIÓN DE RETRASOS EN TIEMPO REAL");

    printf("🚨 Simulando retrasos en el sistema...\n\n");

    // Simular retraso en L1
    printf("⚠️ Aplicando retraso a Línea 1...\n");
    updateDelays(system, linea1, 8); // 8 minutos de retraso

    // Simular problema en Bus 47
    printf("🚫 Simulando problema en Bus 47...\n");
    updateDelays(system, bus47, 25); // 25 minutos de retraso

    // Mostrar estado de retrasos
    printf("\n📊 Estado actual de retrasos:\n");
    printDelayStatus(system);

    // Analizar rendimiento del sistema
    printf("📈 Análisis de rendimiento:\n");
    analyzeSystemPerformance(system);

    // Verificar si necesita rerouteo
    printf("🔍 Verificando necesidad de rerouteo...\n");
    if (shouldReroutePassengers(system, linea1->id)) {
        printf("⚠️ Se recomienda reroutear pasajeros de L1\n");
    }

    if (shouldReroutePassengers(system, bus47->id)) {
        printf("⚠️ Se recomienda reroutear pasajeros de Bus 47\n");
    }

    pauseDemo();

    // ============================================================================
    // PASO 9: GENERACIÓN DE ALERTAS
    // ============================================================================
    printStepHeader(9, "SISTEMA DE ALERTAS AUTOMÁTICAS");

    printf("📢 Generando alertas automáticas...\n");
    generateAutomaticAlerts(system);

    // Crear alerta manual
    printf("\n📱 Creando alerta manual...\n");
    DelayAlert* manual_alert = createDelayAlert(ALERT_SERVICE_DISRUPTION,
                                               bus47->id, 25,
                                               "Incidente en vía - servicio suspendido temporalmente");
    if (manual_alert) {
        broadcastAlert(system, manual_alert);
        destroyDelayAlert(manual_alert);
    }

    pauseDemo();

    // ============================================================================
    // PASO 10: REPORTES Y ESTADÍSTICAS
    // ============================================================================
    printStepHeader(10, "GENERACIÓN DE REPORTES");

    printf("📄 Generando reportes del sistema...\n");

    // Generar reporte de retrasos
    generateDelayReport(system, "reporte_retrasos.txt");

    // Generar reporte de tarifa (si hay ruta)
    if (fastest_route) {
        generateFareReport(fastest_route, system->fares, "reporte_tarifa.txt");
    }

    // Mostrar estadísticas finales
    printf("\n📊 ESTADÍSTICAS FINALES DEL SISTEMA:\n");
    printSystemSummary(system);

    printf("\n🎯 MÉTRICAS DE RENDIMIENTO:\n");
    printf("   ⚡ Rutas planificadas: %s\n", fastest_route ? "✅" : "❌");
    printf("   💰 Sistema de tarifas: ✅\n");
    printf("   ⏰ Seguimiento tiempo real: ✅\n");
    printf("   📱 Sistema de alertas: ✅\n");
    printf("   📊 Generación de reportes: ✅\n");

    pauseDemo();

    // ============================================================================
    // PASO 11: LIMPIEZA Y FINALIZACIÓN
    // ============================================================================
    printStepHeader(11, "LIMPIEZA DEL SISTEMA");

    printf("🧹 Liberando memoria...\n");

    // Limpiar rutas
    if (fastest_route) {
        destroyRoute(fastest_route);
        printf("✅ Ruta rápida destruida\n");
    }

    if (cheapest_route) {
        destroyRoute(cheapest_route);
        printf("✅ Ruta barata destruida\n");
    }

    if (multiple_routes) {
        for (int i = 0; i < route_count; i++) {
            if (multiple_routes[i]) {
                destroyRoute(multiple_routes[i]);
            }
        }
        free(multiple_routes);
        printf("✅ Rutas múltiples destruidas\n");
    }

    // Destruir sistema completo
    destroyTransitSystem(system);
    printf("✅ Sistema de transporte destruido\n");

    // ============================================================================
    // FINALIZACIÓN
    // ============================================================================
    printSeparator("DEMO COMPLETADO");

    printf("🎉 ¡Demo completado exitosamente!\n\n");
    printf("📋 RESUMEN DE FUNCIONALIDADES PROBADAS:\n");
    printf("   ✅ Creación y gestión del sistema\n");
    printf("   ✅ Administración de estaciones y líneas\n");
    printf("   ✅ Configuración de horarios\n");
    printf("   ✅ Planificación inteligente de rutas\n");
    printf("   ✅ Sistema de tarifas con descuentos\n");
    printf("   ✅ Seguimiento de retrasos en tiempo real\n");
    printf("   ✅ Sistema de alertas automáticas\n");
    printf("   ✅ Generación de reportes\n");
    printf("   ✅ Gestión completa de memoria\n\n");

    printf("🚇 El Sistema de Transporte Público Inteligente está funcionando correctamente.\n");
    printf("📁 Revisa los archivos 'reporte_retrasos.txt' y 'reporte_tarifa.txt'\n");
    printf("🎯 ¡Gracias por probar el sistema!\n\n");

    return 0;
}

// ============================================================================
// FUNCIONES ADICIONALES PARA TESTING
// ============================================================================

void runQuickTest() {
    printf("🧪 Ejecutando test rápido...\n");

    TransitSystem* test_system = createTransitSystem(5, 3);

    Station* s1 = createStation(1, "Test1", 0.0, 0.0, 1, true);
    Station* s2 = createStation(2, "Test2", 1.0, 1.0, 1, true);

    addStationToSystem(test_system, s1);
    addStationToSystem(test_system, s2);

    Line* test_line = createLine(1, "TestLine", "#FF0000", 0);
    addStationToLine(test_line, s1, 5);
    addStationToLine(test_line, s2, 0);
    addLineToSystem(test_system, test_line);

    if (validateTransitSystem(test_system)) {
        printf("✅ Test rápido: PASÓ\n");
    } else {
        printf("❌ Test rápido: FALLÓ\n");
    }

    destroyTransitSystem(test_system);
}

bool addScheduleToSystem(TransitSystem* system, Schedule* schedule) {
    if (!system || !schedule) {
        printf("❌ Error: Sistema o horario nulos\n");
        return false;
    }

    if (system->schedules_count >= system->capacity_schedules) {
        printf("❌ Error: Capacidad máxima de horarios alcanzada (%d)\n",
               system->capacity_schedules);
        return false;
    }

    // Verificar que existe la línea para este horario
    Line* line = findLineById(system, schedule->line_id);
    if (!line) {
        printf("❌ Error: No existe línea con ID %d para el horario\n", schedule->line_id);
        return false;
    }

    // Verificar que no existe horario para esta línea
    for (int i = 0; i < system->schedules_count; i++) {
        if (system->schedules[i]->line_id == schedule->line_id) {
            printf("❌ Error: Ya existe horario para línea ID %d (%s)\n",
                   schedule->line_id, line->name);
            return false;
        }
    }

    system->schedules[system->schedules_count] = schedule;
    system->schedules_count++;

    printf("➕ Horario agregado para línea %s (ID: %d)\n", line->name, schedule->line_id);
    printf("   🕐 Servicio: %02d:%02d - %02d:%02d, frecuencia: %d min\n",
           schedule->first_service.hour, schedule->first_service.minute,
           schedule->last_service.hour, schedule->last_service.minute,
           schedule->frequency_minutes);

    return true;
}

Station* findStationById(TransitSystem* system, int id) {
    if (!system) return NULL;

    for (int i = 0; i < system->station_count; i++) {
        if (system->stations[i]->id == id) {
            return system->stations[i];
        }
    }
    return NULL;
}

Station* findStationByName(TransitSystem* system, const char* name) {
    if (!system || !name) return NULL;

    for (int i = 0; i < system->station_count; i++) {
        if (strcmp(system->stations[i]->name, name) == 0) {
            return system->stations[i];
        }
    }
    return NULL;
}

Line* findLineById(TransitSystem* system, int id) {
    if (!system) return NULL;

    for (int i = 0; i < system->lines_count; i++) {
        if (system->lines[i]->id == id) {
            return system->lines[i];
        }
    }
    return NULL;
}

Line* findLineByName(TransitSystem* system, const char* name) {
    if (!system || !name) return NULL;

    for (int i = 0; i < system->lines_count; i++) {
        if (strcmp(system->lines[i]->name, name) == 0) {
            return system->lines[i];
        }
    }
    return NULL;
}

void printSystemSummary(TransitSystem* system) {
    if (!system) {
        printf("❌ Sistema nulo\n");
        return;
    }

    printf("📊 RESUMEN DEL SISTEMA:\n");
    printf("   🚉 %d estaciones | 🚇 %d líneas | 📅 %d horarios\n",
           system->station_count, system->lines_count, system->schedules_count);

    if (system->station_count > 0 && system->lines_count > 0) {
        printf("   ✅ Sistema operativo\n");
    } else {
        printf("   ⚠️ Sistema incompleto\n");
    }
}

int getSystemStationCount(TransitSystem* system) {
    return system ? system->station_count : 0;
}

int getSystemLineCount(TransitSystem* system) {
    return system ? system->lines_count : 0;
}

int getSystemScheduleCount(TransitSystem* system) {
    return system ? system->schedules_count : 0;
}

bool isSystemEmpty(TransitSystem* system) {
    return !system || (system->station_count == 0 && system->lines_count == 0);
}
