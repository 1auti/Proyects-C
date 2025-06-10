//
// Created by administrador on 5/28/25.
//

#include "station.h"

Station * createStation(int id, const char * name, double lat, double lon, int zone, bool accesible) {

    Station * station = (Station*)malloc(sizeof(Station));
    if(!station) return NULL;

    station->id = id;

    strncpy(station->name, name, sizeof(station->name) - 1);
    station->name[sizeof(station->name) - 1] = '\0';
    station->latitude = lat;
    station->longitude = lon;
    station->zone = zone;
    station->accesible = accesible;

    // Inicializamos los array
    station->capacity_lines = 10;
    station->lines = (char**)malloc(station->capacity_lines * sizeof(char*));
    station->line_count = 0;

    if(!station->lines) {
        free(station);
        return NULL;
    }
    return station;
}

void destroyStation(Station * station) {
    if(!station) return;

    //Liberar nombres de lineas
    for(int i = 0 ; i < station->line_count; i++) {
        free(station->lines[i]);
    }
    free(station->lines);
    free(station);
}

bool addLineToStation(Station * station, const char * line_name) {
    if(!station || !line_name || station->line_count >= station->capacity_lines) {
        return false;
    }

    // Verificar que la linea no esta existiendo
    for(int i = 0; i < station->line_count; i++) {
        if(strcmp(station->lines[i], line_name)==0) {
            return  false; // YA EXISTE
        }
    }

    // Agremos nuevas lineas
    station->lines[station->line_count] = (char*)malloc(strlen(line_name) + 1 );
    if(!station->lines[station->line_count]) {
        return false;
    }

    strcpy(station->lines[station->line_count],line_name);
    station->line_count++;

    return  true;
}

void printStation(Station* station) {
    if (!station) return;

    printf("🚉 Estación: %s (ID: %d)\n", station->name, station->id);
    printf("   📍 Coordenadas: %.6f, %.6f\n", station->latitude, station->longitude);
    printf("   🎫 Zona: %d\n", station->zone);
    printf("   ♿ Accesible: %s\n", station->accesible ? "Sí" : "No");
    printf("   🚇 Líneas (%d): ", station->line_count);
    for (int i = 0; i < station->line_count; i++) {
        printf("%s%s", station->lines[i], (i < station->line_count - 1) ? ", " : "");
    }
    printf("\n");
}