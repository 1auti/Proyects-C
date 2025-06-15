// ============================================================================
// realtime/delay_tracker.c - Seguimiento de retrasos
// ============================================================================
#include "delay_tracker.h"
#include "../core/transit_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// ============================================================================
// FUNCIONES PRINCIPALES DE SEGUIMIENTO DE RETRASOS
// ============================================================================

void updateDelays(TransitSystem* system, Line* line, int delay_minutes) {
    if (!system || !line) {
        printf("❌ Error: Sistema o línea nulos\n");
        return;
    }

    RealTimeData* delay_data = NULL;

    // Buscar datos existentes para esta línea
    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->line_id == line->id) {
            delay_data = system->delays[i];
            break;
        }
    }

    // Crear nuevo registro si no existe
    if (!delay_data) {
        if (system->delay_count >= system->capacity_delays) {
            printf("❌ Error: Capacidad máxima de retrasos alcanzada (%d)\n",
                   system->capacity_delays);
            return;
        }

        delay_data = createRealTimeData(line->id);
        if (!delay_data) {
            printf("❌ Error: No se pudo crear datos de tiempo real\n");
            return;
        }

        system->delays[system->delay_count++] = delay_data;
        printf("📊 Nuevo registro de retraso creado para línea %s\n", line->name);
    }

    // Actualizar información de retraso
    delay_data->delay_minutes = delay_minutes;
    delay_data->service_active = (delay_minutes < 60); // Inactivo si retraso > 1 hora

    // Crear mensaje de estado apropiado
    if (delay_minutes > 30) {
        snprintf(delay_data->status_message, sizeof(delay_data->status_message),
                "Línea %s: RETRASO SEVERO de %d minutos", line->name, delay_minutes);
        printf("🚨 %s\n", delay_data->status_message);
    } else if (delay_minutes > 10) {
        snprintf(delay_data->status_message, sizeof(delay_data->status_message),
                "Línea %s: Retraso moderado de %d minutos", line->name, delay_minutes);
        printf("⚠️ %s\n", delay_data->status_message);
    } else if (delay_minutes > 0) {
        snprintf(delay_data->status_message, sizeof(delay_data->status_message),
                "Línea %s: Retraso leve de %d minutos", line->name, delay_minutes);
        printf("🟡 %s\n", delay_data->status_message);
    } else {
        snprintf(delay_data->status_message, sizeof(delay_data->status_message),
                "Línea %s: Servicio normal", line->name);
        printf("✅ %s\n", delay_data->status_message);
    }

    // Actualizar timestamp (simulado)
    // En implementación real, usarías getCurrentDateTime()
    printf("⏰ Retraso actualizado a las %02d:%02d\n",
           (int)(time(NULL) % 86400) / 3600, ((int)(time(NULL) % 3600)) / 60);
}

int getCurrentDelay(TransitSystem* system, int line_id) {
    if (!system) {
        printf("❌ Error: Sistema nulo\n");
        return 0;
    }

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->line_id == line_id) {
            return system->delays[i]->delay_minutes;
        }
    }

    // No hay retraso registrado
    return 0;
}

bool hasActiveDelays(TransitSystem* system) {
    if (!system) return false;

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->delay_minutes > 0) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// GESTIÓN DE RETRASOS
// ============================================================================

void clearOldDelays(TransitSystem* system, int max_age_minutes) {
    if (!system) return;

    printf("🧹 Limpiando retrasos antiguos (más de %d minutos)...\n", max_age_minutes);

    int cleared_count = 0;

    for (int i = 0; i < system->delay_count; i++) {
        RealTimeData* delay = system->delays[i];

        // Simulación simple: retrasos mayores al límite se consideran "antiguos"
        if (delay->delay_minutes > max_age_minutes) {
            delay->delay_minutes = 0;
            strcpy(delay->status_message, "Servicio normal");
            delay->service_active = true;
            cleared_count++;
        }
    }

    if (cleared_count > 0) {
        printf("✅ Se limpiaron %d retrasos antiguos\n", cleared_count);
    } else {
        printf("ℹ️ No se encontraron retrasos antiguos para limpiar\n");
    }
}

void clearAllDelays(TransitSystem* system) {
    if (!system) return;

    printf("🧹 Limpiando todos los retrasos del sistema...\n");

    for (int i = 0; i < system->delay_count; i++) {
        system->delays[i]->delay_minutes = 0;
        strcpy(system->delays[i]->status_message, "Servicio normal");
        system->delays[i]->service_active = true;
    }

    printf("✅ Todos los retrasos han sido limpiados (%d líneas afectadas)\n",
           system->delay_count);
}

void setLineStatus(TransitSystem* system, int line_id, bool service_active) {
    if (!system) return;

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->line_id == line_id) {
            system->delays[i]->service_active = service_active;

            if (!service_active) {
                strcpy(system->delays[i]->status_message, "Servicio suspendido");
                printf("🚫 Línea ID %d: Servicio suspendido\n", line_id);
            } else {
                strcpy(system->delays[i]->status_message, "Servicio restablecido");
                printf("✅ Línea ID %d: Servicio restablecido\n", line_id);
            }
            return;
        }
    }

    printf("⚠️ No se encontraron datos para línea ID %d\n", line_id);
}

// ============================================================================
// CONSULTAS DE ESTADO
// ============================================================================

bool isLineActive(TransitSystem* system, int line_id) {
    if (!system) return false;

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->line_id == line_id) {
            return system->delays[i]->service_active;
        }
    }

    // Si no hay datos de retraso, asumimos que está activa
    return true;
}

char* getLineStatus(TransitSystem* system, int line_id) {
    static char default_status[] = "Estado desconocido";

    if (!system) return default_status;

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->line_id == line_id) {
            return system->delays[i]->status_message;
        }
    }

    return default_status;
}

int getAverageDelay(TransitSystem* system) {
    if (!system || system->delay_count == 0) return 0;

    int total_delay = 0;
    int active_delays = 0;

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->delay_minutes > 0) {
            total_delay += system->delays[i]->delay_minutes;
            active_delays++;
        }
    }

    if (active_delays == 0) return 0;

    int average = total_delay / active_delays;
    printf("📊 Retraso promedio del sistema: %d minutos (%d líneas afectadas)\n",
           average, active_delays);

    return average;
}

int getMaxDelay(TransitSystem* system) {
    if (!system || system->delay_count == 0) return 0;

    int max_delay = 0;
    int worst_line_id = -1;

    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->delay_minutes > max_delay) {
            max_delay = system->delays[i]->delay_minutes;
            worst_line_id = system->delays[i]->line_id;
        }
    }

    if (max_delay > 0) {
        printf("📊 Retraso máximo: %d minutos (Línea ID %d)\n", max_delay, worst_line_id);
    }

    return max_delay;
}

// ============================================================================
// ANÁLISIS DE IMPACTO
// ============================================================================

double calculateDelayImpact(int delay_minutes, Schedule* schedule) {
    if (!schedule || delay_minutes <= 0) return 0.0;

    // Impacto = retraso / frecuencia (qué porcentaje de la frecuencia representa el retraso)
    double impact = (double)delay_minutes / schedule->frequency_minutes;

    // Penalizar más los retrasos en horas pico
    if (impact > 1.0) {
        impact *= 1.5; // Impacto severo
    }

    printf("📈 Impacto del retraso: %.2f (retraso %d min / frecuencia %d min)\n",
           impact, delay_minutes, schedule->frequency_minutes);

    return impact;
}

int estimateDelayDuration(int current_delay, int historical_average) {
    if (current_delay <= 0) return 0;

    // Estimación basada en patrón histórico
    int estimated_duration;

    if (current_delay <= 5) {
        estimated_duration = historical_average * 0.5; // Retrasos leves se resuelven rápido
    } else if (current_delay <= 15) {
        estimated_duration = historical_average * 0.8; // Retrasos moderados
    } else {
        estimated_duration = historical_average * 1.2; // Retrasos severos tardan más
    }

    // Mínimo 5 minutos
    if (estimated_duration < 5) estimated_duration = 5;

    printf("⏱️ Duración estimada del retraso: %d minutos\n", estimated_duration);

    return estimated_duration;
}

bool shouldReroutePassengers(TransitSystem* system, int line_id) {
    if (!system) return false;

    int delay = getCurrentDelay(system, line_id);
    bool is_active = isLineActive(system, line_id);

    // Recomendar reroute si:
    // 1. Retraso > 20 minutos
    // 2. Servicio inactivo
    // 3. Retraso > promedio del sistema * 2

    if (!is_active) {
        printf("🚨 RECOMENDACIÓN: Rerutar pasajeros - servicio suspendido\n");
        return true;
    }

    if (delay > 20) {
        printf("🚨 RECOMENDACIÓN: Rerutar pasajeros - retraso severo (%d min)\n", delay);
        return true;
    }

    int avg_delay = getAverageDelay(system);
    if (delay > avg_delay * 2 && avg_delay > 0) {
        printf("🚨 RECOMENDACIÓN: Rerutar pasajeros - retraso anómalo (%d min vs %d promedio)\n",
               delay, avg_delay);
        return true;
    }

    return false;
}

// ============================================================================
// NOTIFICACIONES Y ALERTAS
// ============================================================================

DelayAlert* createDelayAlert(AlertType type, int line_id, int delay_minutes, const char* message) {
    DelayAlert* alert = (DelayAlert*)malloc(sizeof(DelayAlert));
    if (!alert) {
        printf("❌ Error: No se pudo crear alerta\n");
        return NULL;
    }

    alert->type = type;
    alert->line_id = line_id;
    alert->delay_minutes = delay_minutes;

    // Timestamp simulado (en implementación real usarías getCurrentDateTime())
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    alert->timestamp.date.day = timeinfo->tm_mday;
    alert->timestamp.date.month = timeinfo->tm_mon + 1;
    alert->timestamp.date.year = timeinfo->tm_year + 1900;
    alert->timestamp.time.hour = timeinfo->tm_hour;
    alert->timestamp.time.minute = timeinfo->tm_min;

    // Copiar mensaje
    if (message) {
        strncpy(alert->message, message, sizeof(alert->message) - 1);
        alert->message[sizeof(alert->message) - 1] = '\0';
    } else {
        strcpy(alert->message, "Sin mensaje");
    }

    const char* alert_types[] = {"Retraso Leve", "Retraso Severo", "Servicio Interrumpido", "Servicio Restablecido"};
    printf("🚨 Alerta creada: %s (Línea %d)\n", alert_types[type], line_id);

    return alert;
}

void destroyDelayAlert(DelayAlert* alert) {
    if (alert) {
        printf("🗑️ Alerta destruida\n");
        free(alert);
    }
}

void broadcastAlert(TransitSystem* system, DelayAlert* alert) {
    if (!system || !alert) return;

    const char* alert_types[] = {"RETRASO LEVE", "RETRASO SEVERO", "SERVICIO INTERRUMPIDO", "SERVICIO RESTABLECIDO"};
    const char* icons[] = {"🟡", "🟠", "🚫", "✅"};

    printf("\n📢 ===== ALERTA DEL SISTEMA =====\n");
    printf("%s %s\n", icons[alert->type], alert_types[alert->type]);
    printf("🚇 Línea: %d\n", alert->line_id);

    if (alert->delay_minutes > 0) {
        printf("⏰ Retraso: %d minutos\n", alert->delay_minutes);
    }

    printf("📅 Hora: %02d:%02d del %02d/%02d/%04d\n",
           alert->timestamp.time.hour, alert->timestamp.time.minute,
           alert->timestamp.date.day, alert->timestamp.date.month, alert->timestamp.date.year);

    printf("💬 Mensaje: %s\n", alert->message);
    printf("==============================\n\n");

    // En implementación real, aquí enviarías notificaciones push, emails, etc.
}

void generateAutomaticAlerts(TransitSystem* system) {
    if (!system) return;

    printf("🔍 Generando alertas automáticas...\n");

    for (int i = 0; i < system->delay_count; i++) {
        RealTimeData* delay = system->delays[i];
        AlertType alert_type;
        char alert_message[200];

        if (!delay->service_active) {
            alert_type = ALERT_SERVICE_DISRUPTION;
            snprintf(alert_message, sizeof(alert_message),
                    "Servicio suspendido en línea %d", delay->line_id);
        } else if (delay->delay_minutes >= 20) {
            alert_type = ALERT_MAJOR_DELAY;
            snprintf(alert_message, sizeof(alert_message),
                    "Retraso severo de %d minutos en línea %d",
                    delay->delay_minutes, delay->line_id);
        } else if (delay->delay_minutes >= 5) {
            alert_type = ALERT_MINOR_DELAY;
            snprintf(alert_message, sizeof(alert_message),
                    "Retraso leve de %d minutos en línea %d",
                    delay->delay_minutes, delay->line_id);
        } else {
            continue; // No generar alerta para retrasos menores
        }

        DelayAlert* alert = createDelayAlert(alert_type, delay->line_id,
                                           delay->delay_minutes, alert_message);
        if (alert) {
            broadcastAlert(system, alert);
            destroyDelayAlert(alert);
        }
    }
}

// ============================================================================
// UTILIDADES DE VISUALIZACIÓN
// ============================================================================

void printDelayStatus(TransitSystem* system) {
    if (!system) {
        printf("❌ Sistema nulo\n");
        return;
    }

    printf("\n⚠️ ===== ESTADO DE RETRASOS =====\n");
    printf("📊 Líneas monitoreadas: %d\n", system->delay_count);

    if (system->delay_count == 0) {
        printf("✅ No hay líneas monitoreadas\n");
        printf("===============================\n\n");
        return;
    }

    int active_delays = 0;
    int suspended_lines = 0;

    for (int i = 0; i < system->delay_count; i++) {
        RealTimeData* delay = system->delays[i];

        const char* status_icon;
        if (!delay->service_active) {
            status_icon = "🚫";
            suspended_lines++;
        } else if (delay->delay_minutes >= 20) {
            status_icon = "🔴";
            active_delays++;
        } else if (delay->delay_minutes >= 10) {
            status_icon = "🟠";
            active_delays++;
        } else if (delay->delay_minutes > 0) {
            status_icon = "🟡";
            active_delays++;
        } else {
            status_icon = "✅";
        }

        printf("%s Línea %d: %s\n",
               status_icon, delay->line_id, delay->status_message);
    }

    printf("\n📈 RESUMEN:\n");
    printf("   🟡 Líneas con retrasos: %d\n", active_delays);
    printf("   🚫 Líneas suspendidas: %d\n", suspended_lines);
    printf("   ✅ Líneas normales: %d\n",
           system->delay_count - active_delays - suspended_lines);

    if (active_delays > 0) {
        printf("   📊 Retraso promedio: %d minutos\n", getAverageDelay(system));
        printf("   📊 Retraso máximo: %d minutos\n", getMaxDelay(system));
    }

    printf("===============================\n\n");
}

void printLineDelayHistory(TransitSystem* system, int line_id) {
    if (!system) return;

    printf("\n📈 ===== HISTORIAL LÍNEA %d =====\n", line_id);

    // Buscar datos de la línea
    RealTimeData* delay_data = NULL;
    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->line_id == line_id) {
            delay_data = system->delays[i];
            break;
        }
    }

    if (!delay_data) {
        printf("❌ No se encontraron datos para línea %d\n", line_id);
        printf("==============================\n\n");
        return;
    }

    printf("📊 Estado actual: %s\n", delay_data->status_message);
    printf("⏰ Retraso actual: %d minutos\n", delay_data->delay_minutes);
    printf("🔄 Servicio activo: %s\n", delay_data->service_active ? "SÍ" : "NO");

    // En implementación real, mostrarías historial completo
    printf("📋 Historial simulado (últimas 24 horas):\n");
    printf("   08:00 - Servicio normal\n");
    printf("   09:15 - Retraso 5 min\n");
    printf("   12:30 - Servicio normal\n");
    printf("   17:45 - Retraso 8 min\n");
    printf("   18:20 - Servicio normal\n");
    printf("   Actual - %s\n", delay_data->status_message);

    printf("==============================\n\n");
}


// ============================================================================
// FUNCIONES DE ANÁLISIS AVANZADO
// ============================================================================



void analyzeSystemPerformance(TransitSystem* system) {
    if (!system) return;

    printf("\n📊 ===== ANÁLISIS DE RENDIMIENTO =====\n");

    if (system->delay_count == 0) {
        printf("ℹ️ No hay datos suficientes para análisis\n");
        printf("====================================\n\n");
        return;
    }

    int total_delays = 0;
    int severe_delays = 0;
    int suspended_lines = 0;
    int normal_lines = 0;

    for (int i = 0; i < system->delay_count; i++) {
        RealTimeData* delay = system->delays[i];

        if (!delay->service_active) {
            suspended_lines++;
        } else if (delay->delay_minutes >= 20) {
            severe_delays++;
            total_delays++;
        } else if (delay->delay_minutes > 0) {
            total_delays++;
        } else {
            normal_lines++;
        }
    }

    double performance_score = ((double)normal_lines / system->delay_count) * 100;

    printf("📈 MÉTRICAS:\n");
    printf("   Líneas funcionando normalmente: %d (%.1f%%)\n",
           normal_lines, ((double)normal_lines / system->delay_count) * 100);
    printf("   Líneas con retrasos: %d (%.1f%%)\n",
           total_delays, ((double)total_delays / system->delay_count) * 100);
    printf("   Líneas con retrasos severos: %d (%.1f%%)\n",
           severe_delays, ((double)severe_delays / system->delay_count) * 100);
    printf("   Líneas suspendidas: %d (%.1f%%)\n",
           suspended_lines, ((double)suspended_lines / system->delay_count) * 100);

    printf("\n🎯 PUNTUACIÓN DEL SISTEMA: %.1f/100\n", performance_score);

    if (performance_score >= 90) {
        printf("✅ Rendimiento excelente\n");
    } else if (performance_score >= 75) {
        printf("🟡 Rendimiento bueno\n");
    } else if (performance_score >= 50) {
        printf("🟠 Rendimiento regular - requiere atención\n");
    } else {
        printf("🔴 Rendimiento deficiente - intervención urgente\n");
    }

    printf("====================================\n\n");
}

void generateAutomaticAlerts(TransitSystem* system) {
    if (!system) return;

    printf("🔍 Generando alertas automáticas...\n");

    for (int i = 0; i < system->delay_count; i++) {
        RealTimeData* delay = system->delays[i];
        AlertType alert_type;
        char alert_message[200];

        if (!delay->service_active) {
            alert_type = ALERT_SERVICE_DISRUPTION;
            snprintf(alert_message, sizeof(alert_message),
                    "Servicio suspendido en línea %d", delay->line_id);
        } else if (delay->delay_minutes >= 20) {
            alert_type = ALERT_MAJOR_DELAY;
            snprintf(alert_message, sizeof(alert_message),
                    "Retraso severo de %d minutos en línea %d",
                    delay->delay_minutes, delay->line_id);
        } else if (delay->delay_minutes >= 5) {
            alert_type = ALERT_MINOR_DELAY;
            snprintf(alert_message, sizeof(alert_message),
                    "Retraso leve de %d minutos en línea %d",
                    delay->delay_minutes, delay->line_id);
        } else {
            continue; // No generar alerta para retrasos menores
        }

        DelayAlert* alert = createDelayAlert(alert_type, delay->line_id,
                                           delay->delay_minutes, alert_message);
        if (alert) {
            broadcastAlert(system, alert);
            destroyDelayAlert(alert);
        }
    }
}

void generateDelayReport(TransitSystem* system, const char* filename) {
    if (!system || !filename) {
        printf("❌ Error: Parámetros nulos para generar reporte\n");
        return;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("❌ Error: No se pudo crear archivo de reporte\n");
        return;
    }

    time_t now = time(NULL);

    fprintf(file, "=== REPORTE DE RETRASOS ===\n");
    fprintf(file, "Fecha: %s", ctime(&now));
    fprintf(file, "Sistema: %d líneas monitoreadas\n\n", system->delay_count);

    fprintf(file, "ESTADO POR LÍNEA:\n");
    for (int i = 0; i < system->delay_count; i++) {
        RealTimeData* delay = system->delays[i];
        fprintf(file, "Línea %d: %s (%d min, %s)\n",
                delay->line_id,
                delay->status_message,
                delay->delay_minutes,
                delay->service_active ? "ACTIVO" : "SUSPENDIDO");
    }

    fprintf(file, "\nESTADÍSTICAS:\n");
    fprintf(file, "Retraso promedio: %d minutos\n", getAverageDelay(system));
    fprintf(file, "Retraso máximo: %d minutos\n", getMaxDelay(system));

    int active_delays = 0;
    for (int i = 0; i < system->delay_count; i++) {
        if (system->delays[i]->delay_minutes > 0) active_delays++;
    }
    fprintf(file, "Líneas afectadas: %d/%d\n", active_delays, system->delay_count);

    fclose(file);
    printf("📄 Reporte generado: %s\n", filename);
}