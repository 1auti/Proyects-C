//
// Created by administrador on 5/29/25.
//

#ifndef DELAY_TRACER_H
#define DELAY_TRACER_H

#include "realtime_data.h"
#include "../core/line.h"
#include "../utils/time_utils.h"
#include <stdbool.h>
#include "../scheduling/schedule.h"

// Forward declaration para evitar dependencias circulares
typedef struct TransitSystem TransitSystem;

// Funciones principales de seguimiento de retrasos
void updateDelays(TransitSystem* system, Line* line, int delay_minutes);
int getCurrentDelay(TransitSystem* system, int line_id);
bool hasActiveDelays(TransitSystem* system);

// Gestión de retrasos
void clearOldDelays(TransitSystem* system, int max_age_minutes);
void clearAllDelays(TransitSystem* system);
void setLineStatus(TransitSystem* system, int line_id, bool service_active);

// Consultas de estado
bool isLineActive(TransitSystem* system, int line_id);
char* getLineStatus(TransitSystem* system, int line_id);
int getAverageDelay(TransitSystem* system);
int getMaxDelay(TransitSystem* system);

// Análisis de impacto
double calculateDelayImpact(int delay_minutes, Schedule* schedule);
int estimateDelayDuration(int current_delay, int historical_average);
bool shouldReroutePassengers(TransitSystem* system, int line_id);

// Notificaciones y alertas
typedef enum {
    ALERT_MINOR_DELAY = 0,
    ALERT_MAJOR_DELAY,
    ALERT_SERVICE_DISRUPTION,
    ALERT_SERVICE_RESTORED
} AlertType;

typedef struct DelayAlert {
    AlertType type;
    int line_id;
    int delay_minutes;
    DateTime timestamp;
    char message[200];
} DelayAlert;

DelayAlert* createDelayAlert(AlertType type, int line_id, int delay_minutes, const char* message);
void destroyDelayAlert(DelayAlert* alert);
void broadcastAlert(TransitSystem* system, DelayAlert* alert);

// Utilidades de visualización
void printDelayStatus(TransitSystem* system);
void printLineDelayHistory(TransitSystem* system, int line_id);
void generateDelayReport(TransitSystem* system, const char* filename);

// Agregar a delay_tracker.h:
void analyzeSystemPerformance(TransitSystem* system);
void generateAutomaticAlerts(TransitSystem* system);


#endif //DELAY_TRACER_H
