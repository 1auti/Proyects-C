//
// Created by administrador on 5/28/25.
//

#ifndef PRICE_MATRIX_H
#define PRICE_MATRIX_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Matriz de precios por zonas
typedef struct PriceMatrix{

 double ** prices; // Matriz de precios entre zonas
 int zone_count; // Numero de zonas
 double base_fare; // Tarifa base
 double transfer_penalty; // Penalizacion por transbordo
}PriceMatrix;

PriceMatrix* createPriceMatrix(int zone_count, double base_fare, double transfer_penalty);
void destroyPriceMatrix(PriceMatrix* matrix);
double getPriceForZones(PriceMatrix* matrix, int from_zone, int to_zone);
bool setPriceForZones(PriceMatrix* matrix, int from_zone, int to_zone, double price);

#endif //PRICE_MATRIX_H
