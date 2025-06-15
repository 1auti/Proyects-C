// ============================================================================
// pricing/fare_calculator.c - Calculadora de tarifas
// ============================================================================
#include "fare_calculator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

// ============================================================================
// CÁLCULOS BÁSICOS DE TARIFAS
// ============================================================================

double calculateBaseFare(Station* from, Station* to, PriceMatrix* matrix) {
    if (!from || !to || !matrix) {
        printf("❌ Error: Parámetros nulos para cálculo de tarifa base\n");
        return -1.0;
    }

    double fare = getPriceForZones(matrix, from->zone, to->zone);

    if (fare < 0) {
        printf("❌ Error: No se pudo obtener precio para zonas %d → %d\n",
               from->zone, to->zone);
        return matrix->base_fare; // Tarifa base por defecto
    }

    printf("💰 Tarifa base %s (Z%d) → %s (Z%d): $%.2f\n",
           from->name, from->zone, to->name, to->zone, fare);

    return fare;
}

double calculateTransferFare(Route* route, PriceMatrix* matrix) {
    if (!route || !matrix) {
        return 0.0;
    }

    if (route->transfer_count == 0) {
        printf("ℹ️ Viaje directo - sin cargos por transbordo\n");
        return 0.0;
    }

    double transfer_cost = route->transfer_count * matrix->transfer_penalty;

    printf("🔄 Costo por transbordos: %d × $%.2f = $%.2f\n",
           route->transfer_count, matrix->transfer_penalty, transfer_cost);

    return transfer_cost;
}

double calculateDistanceFare(Station* from, Station* to, double rate_per_km) {
    if (!from || !to || rate_per_km <= 0) {
        return 0.0;
    }

    // Calcular distancia usando coordenadas geográficas (aproximación simple)
    double lat_diff = to->latitude - from->latitude;
    double lon_diff = to->longitude - from->longitude;

    // Fórmula haversina simplificada (aproximación)
    double distance_km = sqrt(lat_diff * lat_diff + lon_diff * lon_diff) * 111.0; // ~111 km por grado

    double distance_fare = distance_km * rate_per_km;

    printf("📏 Tarifa por distancia: %.2f km × $%.3f/km = $%.2f\n",
           distance_km, rate_per_km, distance_fare);

    return distance_fare;
}

double calculateTotalFare(Route* route, PriceMatrix* matrix) {
    if (!route || !matrix || route->path_length < 2) {
        printf("❌ Error: Ruta inválida para cálculo de tarifa\n");
        return 0.0;
    }

    Station* origin = route->path[0];
    Station* destination = route->path[route->path_length - 1];

    printf("\n💳 ===== CÁLCULO DE TARIFA =====\n");

    // Tarifa base según zonas
    double base_fare = calculateBaseFare(origin, destination, matrix);

    // Penalización por transbordos
    double transfer_fare = calculateTransferFare(route, matrix);

    // Total inicial
    double total = base_fare + transfer_fare;

    // Mostrar desglose
    printf("📊 DESGLOSE:\n");
    printf("   📍 Tarifa base: $%.2f\n", base_fare);
    printf("   🔄 Transbordos: $%.2f\n", transfer_fare);
    printf("   ➖➖➖➖➖➖➖➖\n");
    printf("   💳 TOTAL: $%.2f\n", total);
    printf("==============================\n\n");

    return total;
}

// ============================================================================
// GESTIÓN DE DESCUENTOS BÁSICOS
// ============================================================================

bool applyDiscount(Route* route, double discount_percentage) {
    if (!route || discount_percentage < 0 || discount_percentage > 100) {
        printf("❌ Error: Parámetros de descuento inválidos\n");
        return false;
    }

    double original_cost = route->total_cost;
    double discount_amount = original_cost * (discount_percentage / 100.0);
    route->total_cost = original_cost - discount_amount;

    printf("🎫 Descuento aplicado: %.1f%%\n", discount_percentage);
    printf("   Original: $%.2f\n", original_cost);
    printf("   Descuento: -$%.2f\n", discount_amount);
    printf("   Final: $%.2f\n", route->total_cost);

    return true;
}

bool applyStudentDiscount(Route* route) {
    printf("🎓 Aplicando descuento estudiantil...\n");
    return applyDiscount(route, 25.0); // 25% descuento estudiante
}

bool applySeniorDiscount(Route* route) {
    printf("👴 Aplicando descuento tercera edad...\n");
    return applyDiscount(route, 30.0); // 30% descuento tercera edad
}

bool applyGroupDiscount(Route* route, int group_size) {
    if (group_size < 5) {
        printf("ℹ️ Grupo muy pequeño para descuento (mínimo 5 personas)\n");
        return false;
    }

    double discount;
    if (group_size >= 15) {
        discount = 25.0; // 25% para grupos grandes
    } else if (group_size >= 10) {
        discount = 20.0; // 20% para grupos medianos
    } else {
        discount = 10.0; // 10% para grupos pequeños
    }

    printf("👥 Aplicando descuento grupal (%d personas)...\n", group_size);
    return applyDiscount(route, discount);
}

// ============================================================================
// TARJETAS DE DESCUENTO
// ============================================================================

DiscountCard* createDiscountCard(DiscountCardType type, const char* holder_name, double discount) {
    DiscountCard* card = (DiscountCard*)malloc(sizeof(DiscountCard));
    if (!card) {
        printf("❌ Error: No se pudo crear tarjeta de descuento\n");
        return NULL;
    }

    card->type = type;
    card->discount_percentage = discount;
    card->is_valid = true;

    // Establecer fecha de vencimiento (1 año desde ahora)
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    card->expiry_month = timeinfo->tm_mon + 1;
    card->expiry_year = timeinfo->tm_year + 1900 + 1; // +1 año

    // Copiar nombre del titular
    strncpy(card->holder_name, holder_name, sizeof(card->holder_name) - 1);
    card->holder_name[sizeof(card->holder_name) - 1] = '\0';

    const char* type_names[] = {"Ninguna", "Estudiante", "Tercera Edad", "Discapacitado", "Mensual", "Anual"};
    printf("💳 Tarjeta creada: %s\n", type_names[type]);
    printf("   Titular: %s\n", holder_name);
    printf("   Descuento: %.1f%%\n", discount);
    printf("   Vence: %02d/%04d\n", card->expiry_month, card->expiry_year);

    return card;
}

void destroyDiscountCard(DiscountCard* card) {
    if (card) {
        printf("🗑️ Tarjeta de %s destruida\n", card->holder_name);
        free(card);
    }
}

bool isCardValid(DiscountCard* card) {
    if (!card || !card->is_valid) {
        return false;
    }

    // Verificación simple de vigencia (deberías usar fecha actual real)
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    int current_year = timeinfo->tm_year + 1900;
    int current_month = timeinfo->tm_mon + 1;

    if (card->expiry_year < current_year ||
        (card->expiry_year == current_year && card->expiry_month < current_month)) {
        printf("❌ Tarjeta de %s vencida (%02d/%04d)\n",
               card->holder_name, card->expiry_month, card->expiry_year);
        return false;
    }

    return true;
}

double calculateDiscountedFare(Route* route, DiscountCard* card, PriceMatrix* matrix) {
    if (!route || !card || !matrix) {
        printf("❌ Error: Parámetros nulos para cálculo con tarjeta\n");
        return 0.0;
    }

    if (!isCardValid(card)) {
        printf("❌ Tarjeta inválida o vencida\n");
        return calculateTotalFare(route, matrix);
    }

    double original_fare = calculateTotalFare(route, matrix);
    double discount_amount = original_fare * (card->discount_percentage / 100.0);
    double final_fare = original_fare - discount_amount;

    const char* type_names[] = {"Ninguna", "Estudiante", "Tercera Edad", "Discapacitado", "Mensual", "Anual"};

    printf("💳 Tarjeta %s aplicada (%s):\n", type_names[card->type], card->holder_name);
    printf("   Descuento: %.1f%% (-$%.2f)\n", card->discount_percentage, discount_amount);
    printf("   Tarifa final: $%.2f\n", final_fare);

    return final_fare;
}

// ============================================================================
// CÁLCULOS ESPECIALES POR HORARIO
// ============================================================================

double calculatePeakHourSurcharge(Route* route, Time departure_time) {
    if (!route) return 0.0;

    // Definir horas pico: 7-9 AM y 5-7 PM
    bool is_morning_peak = (departure_time.hour >= 7 && departure_time.hour < 9);
    bool is_evening_peak = (departure_time.hour >= 17 && departure_time.hour < 19);

    if (is_morning_peak || is_evening_peak) {
        double surcharge = route->total_cost * 0.25; // 25% recargo
        const char* peak_type = is_morning_peak ? "matutina" : "vespertina";

        printf("⏰ Recargo hora pico %s (%02d:%02d): +$%.2f (25%%)\n",
               peak_type, departure_time.hour, departure_time.minute, surcharge);

        return surcharge;
    }

    printf("ℹ️ Horario normal - sin recargo por hora pico\n");
    return 0.0;
}

double calculateNightSurcharge(Route* route, Time departure_time) {
    if (!route) return 0.0;

    // Servicio nocturno: 10 PM - 5 AM
    bool is_night = (departure_time.hour >= 22) || (departure_time.hour <= 5);

    if (is_night) {
        double surcharge = route->total_cost * 0.15; // 15% recargo nocturno

        printf("🌙 Recargo nocturno (%02d:%02d): +$%.2f (15%%)\n",
               departure_time.hour, departure_time.minute, surcharge);

        return surcharge;
    }

    return 0.0;
}

double calculateWeekendDiscount(Route* route, int day_of_week) {
    if (!route) return 0.0;

    // Descuento fin de semana (sábado=6, domingo=0)
    if (day_of_week == 0 || day_of_week == 6) {
        double discount = route->total_cost * 0.10; // 10% descuento
        const char* day_name = (day_of_week == 0) ? "domingo" : "sábado";

        printf("🎉 Descuento fin de semana (%s): -$%.2f (10%%)\n", day_name, discount);

        return -discount; // Negativo porque es descuento
    }

    return 0.0;
}

double calculateHolidayDiscount(Route* route, bool is_holiday) {
    if (!route || !is_holiday) return 0.0;

    double discount = route->total_cost * 0.05; // 5% descuento en feriados

    printf("🎊 Descuento por feriado: -$%.2f (5%%)\n", discount);

    return -discount; // Negativo porque es descuento
}

// ============================================================================
// CÁLCULOS AVANZADOS Y PROMOCIONES
// ============================================================================

double calculateLoyaltyDiscount(Route* route, int trips_this_month) {
    if (!route || trips_this_month < 10) return 0.0;

    double discount_rate;
    if (trips_this_month >= 50) {
        discount_rate = 0.20; // 20% para usuarios muy frecuentes
    } else if (trips_this_month >= 30) {
        discount_rate = 0.15; // 15% para usuarios frecuentes
    } else if (trips_this_month >= 20) {
        discount_rate = 0.10; // 10% para usuarios regulares
    } else {
        discount_rate = 0.05; // 5% para usuarios ocasionales
    }

    double discount = route->total_cost * discount_rate;

    printf("⭐ Descuento por fidelidad (%d viajes): -$%.2f (%.0f%%)\n",
           trips_this_month, discount, discount_rate * 100);

    return -discount;
}

double calculateDistanceBasedFare(Route* route, double total_distance_km) {
    if (!route || total_distance_km <= 0) return route ? route->total_cost : 0.0;

    // Tarifas escalonadas por distancia
    double fare = 0.0;

    if (total_distance_km <= 5.0) {
        fare = 2.50; // Tarifa base hasta 5 km
    } else if (total_distance_km <= 15.0) {
        fare = 2.50 + (total_distance_km - 5.0) * 0.30; // $0.30 por km adicional
    } else if (total_distance_km <= 30.0) {
        fare = 5.50 + (total_distance_km - 15.0) * 0.20; // $0.20 por km adicional
    } else {
        fare = 8.50 + (total_distance_km - 30.0) * 0.15; // $0.15 por km adicional
    }

    printf("📏 Tarifa por distancia (%.1f km): $%.2f\n", total_distance_km, fare);

    return fare;
}

// ============================================================================
// FUNCIONES DE UTILIDAD Y REPORTES
// ============================================================================

void printFareBreakdown(Route* route, PriceMatrix* matrix, DiscountCard* card) {
    if (!route || !matrix) {
        printf("❌ Error: Parámetros nulos para desglose\n");
        return;
    }

    printf("\n💳 ===== DESGLOSE DETALLADO DE TARIFA =====\n");

    // Información de la ruta
    printf("🗺️ Ruta: %s → %s\n",
           route->path[0]->name,
           route->path[route->path_length - 1]->name);
    printf("📏 Estaciones: %d | 🔄 Transbordos: %d\n",
           route->path_length, route->transfer_count);

    // Cálculos base
    double base_fare = calculateBaseFare(route->path[0], route->path[route->path_length - 1], matrix);
    double transfer_fare = calculateTransferFare(route, matrix);
    double subtotal = base_fare + transfer_fare;

    printf("\n📊 CÁLCULOS:\n");
    printf("   Tarifa base: $%.2f\n", base_fare);
    printf("   Transbordos: $%.2f\n", transfer_fare);
    printf("   Subtotal: $%.2f\n", subtotal);

    // Aplicar tarjeta de descuento si existe
    if (card && isCardValid(card)) {
        double discount = subtotal * (card->discount_percentage / 100.0);
        printf("   Descuento (%s): -$%.2f\n", card->holder_name, discount);
        subtotal -= discount;
    }

    printf("   ➖➖➖➖➖➖➖➖➖\n");
    printf("   💰 TOTAL FINAL: $%.2f\n", subtotal);
    printf("========================================\n\n");
}

bool validateFareCalculation(double calculated_fare, double expected_min, double expected_max) {
    if (calculated_fare < expected_min || calculated_fare > expected_max) {
        printf("⚠️ Advertencia: Tarifa fuera del rango esperado ($%.2f - $%.2f): $%.2f\n",
               expected_min, expected_max, calculated_fare);
        return false;
    }

    printf("✅ Tarifa validada: $%.2f (rango: $%.2f - $%.2f)\n",
           calculated_fare, expected_min, expected_max);
    return true;
}

void generateFareReport(Route* route, PriceMatrix* matrix, const char* filename) {
    if (!route || !matrix || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("❌ Error: No se pudo crear reporte de tarifa\n");
        return;
    }

    time_t now = time(NULL);

    fprintf(file, "=== REPORTE DE TARIFA ===\n");
    fprintf(file, "Fecha: %s", ctime(&now));
    fprintf(file, "Origen: %s (Zona %d)\n", route->path[0]->name, route->path[0]->zone);
    fprintf(file, "Destino: %s (Zona %d)\n",
            route->path[route->path_length - 1]->name,
            route->path[route->path_length - 1]->zone);
    fprintf(file, "Estaciones: %d\n", route->path_length);
    fprintf(file, "Transbordos: %d\n", route->transfer_count);
    fprintf(file, "Costo total: $%.2f\n", route->total_cost);
    fprintf(file, "Tiempo estimado: %d minutos\n", route->total_time_minutes);

    fprintf(file, "\nDESGLOSE:\n");
    double base_fare = calculateBaseFare(route->path[0],
                                        route->path[route->path_length - 1], matrix);
    double transfer_fare = calculateTransferFare(route, matrix);

    fprintf(file, "Tarifa base: $%.2f\n", base_fare);
    fprintf(file, "Transbordos: $%.2f\n", transfer_fare);
    fprintf(file, "Total: $%.2f\n", base_fare + transfer_fare);

    fclose(file);
    printf("📄 Reporte generado: %s\n", filename);
}
