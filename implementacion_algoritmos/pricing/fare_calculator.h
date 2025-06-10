//
// Created by administrador on 5/28/25.
//

#ifndef FARE_CALCULATOR_H
#define FARE_CALCULATOR_H

#include "price_matrix.h"
#include "../core/route.h"
#include "../core/station.h"
#include <stdbool.h>

// Cálculos básicos de tarifas
double calculateBaseFare(Station* from, Station* to, PriceMatrix* matrix);
double calculateTransferFare(Route* route, PriceMatrix* matrix);
double calculateDistanceFare(Station* from, Station* to, double rate_per_km);
double calculateTotalFare(Route* route, PriceMatrix* matrix);

// Gestión de descuentos y promociones
bool applyDiscount(Route* route, double discount_percentage);
bool applyStudentDiscount(Route* route);
bool applySeniorDiscount(Route* route);
bool applyGroupDiscount(Route* route, int group_size);

// Tipos de tarjetas de descuento
typedef enum {
    CARD_NONE = 0,
    CARD_STUDENT,
    CARD_SENIOR,
    CARD_DISABLED,
    CARD_MONTHLY,
    CARD_ANNUAL
} DiscountCardType;

typedef struct DiscountCard {
    DiscountCardType type;
    double discount_percentage;
    char holder_name[100];
    bool is_valid;
    int expiry_month;
    int expiry_year;
} DiscountCard;

// Funciones para tarjetas de descuento
DiscountCard* createDiscountCard(DiscountCardType type, const char* holder_name, double discount);
void destroyDiscountCard(DiscountCard* card);
bool isCardValid(DiscountCard* card);
double calculateDiscountedFare(Route* route, DiscountCard* card, PriceMatrix* matrix);

// Cálculos especiales
double calculatePeakHourSurcharge(Route* route, Time departure_time);
double calculateNightSurcharge(Route* route, Time departure_time);
double calculateWeekendDiscount(Route* route, int day_of_week);

// Agregar a fare_calculator.h:
void generateFareReport(Route* route, PriceMatrix* matrix, const char* filename);
#endif //FARE_CALCULATOR_H
