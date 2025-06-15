//
// Created by administrador on 5/28/25.
//

#include "price_matrix.h"


PriceMatrix* createPriceMatrix(int zone_count, double base_fare, double transfer_penalty) {
    PriceMatrix* matrix = (PriceMatrix*)malloc(sizeof(PriceMatrix));
    if (!matrix) return NULL;

    matrix->zone_count = zone_count;
    matrix->base_fare = base_fare;
    matrix->transfer_penalty = transfer_penalty;

    // Crear matriz 2D
    matrix->prices = (double**)malloc(zone_count * sizeof(double*));
    if (!matrix->prices) {
        free(matrix);
        return NULL;
    }

    for (int i = 0; i < zone_count; i++) {
        matrix->prices[i] = (double*)malloc(zone_count * sizeof(double));
        if (!matrix->prices[i]) {
            // Limpiar memoria parcialmente asignada
            for (int j = 0; j < i; j++) {
                free(matrix->prices[j]);
            }
            free(matrix->prices);
            free(matrix);
            return NULL;
        }

        // Inicializar precios base
        for (int j = 0; j < zone_count; j++) {
            if (i == j) {
                matrix->prices[i][j] = base_fare;
            } else {
                matrix->prices[i][j] = base_fare + (abs(i - j) * 0.5);
            }
        }
    }

    return matrix;
}

void destroyPriceMatrix(PriceMatrix* matrix) {
    if (!matrix) return;

    for (int i = 0; i < matrix->zone_count; i++) {
        free(matrix->prices[i]);
    }
    free(matrix->prices);
    free(matrix);
}

double getPriceForZones(PriceMatrix* matrix, int from_zone, int to_zone) {
    if (!matrix || from_zone < 0 || to_zone < 0 ||
        from_zone >= matrix->zone_count || to_zone >= matrix->zone_count) {
        return -1.0;
    }

    return matrix->prices[from_zone][to_zone];
}

bool setPriceForZones(PriceMatrix* matrix, int from_zone, int to_zone, double price) {
    if (!matrix || from_zone < 0 || to_zone < 0 ||
        from_zone >= matrix->zone_count || to_zone >= matrix->zone_count) {
        return false;
    }

    matrix->prices[from_zone][to_zone] = price;
    return true;
}