//
// Created by administrador on 5/28/25.
//

#ifndef REALTIME_DATA_H
#define REALTIME_DATA_H
#include "../utils/time_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Datos en tiempo real
typedef struct RealTImeData{

 int line_id;
 int delay_minutes; // Retraso de minutos
 DateTime * last_update; // Ultima actualizacion
 bool service_active; // Servicio activo
 char status_message[200]; // Mensaje de estado

}RealTimeData;

RealTimeData* createRealTimeData(int line_id);
void destroyRealTimeData(RealTimeData* data);
void updateRealTimeData(RealTimeData* data, int delay_minutes, const char* message);



#endif //REALTIME_DATA_H
