//
// Created by administrador on 5/28/25.
//


#include "realtime_data.h"

RealTimeData* createRealTimeData(int line_id) {
    RealTimeData* data = (RealTimeData*)malloc(sizeof(RealTimeData));
    if (!data) return NULL;

    data->line_id = line_id;
    data->delay_minutes = 0;
    data->last_update = NULL;  // Se asignará cuando se actualice
    data->service_active = true;
    strcpy(data->status_message, "Servicio normal");

    return data;
}

void destroyRealTimeData(RealTimeData* data) {
    if (!data) return;

    if (data->last_update) {
        free(data->last_update);
    }
    free(data);
}

void updateRealTimeData(RealTimeData* data, int delay_minutes, const char* message) {
    if (!data) return;

    data->delay_minutes = delay_minutes;
    data->service_active = (delay_minutes < 60);

    if (message) {
        strncpy(data->status_message, message, sizeof(data->status_message) - 1);
        data->status_message[sizeof(data->status_message) - 1] = '\0';
    }

    // Aquí podrías actualizar last_update con tiempo actual
    // data->last_update = getCurrentDateTime();
}