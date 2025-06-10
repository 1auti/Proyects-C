//
// Created by administrador on 5/28/25.
//


#include <stdlib.h>
#include "time_utils.h"

// Constructor para Time
Time* newTime(int hour, int minute) {
    Time* t = (Time*)malloc(sizeof(Time));
    if (t != NULL) {
        t->hour = hour;
        t->minute = minute;
    }
    return t;
}

// Constructor para Date
Date* newDate(int day, int month, int year) {
    Date* d = (Date*)malloc(sizeof(Date));
    if (d != NULL) {
        d->day = day;
        d->month = month;
        d->year = year;
    }
    return d;
}

// Constructor para DateTime
DateTime* newDateTime(Date* date, Time* time) {
    DateTime* dt = (DateTime*)malloc(sizeof(DateTime));
    if (dt != NULL && date != NULL && time != NULL) {
        dt->date = *date;
        dt->time = *time;
    }
    return dt;
}

// Compara dos tiempos: devuelve -1 si t1 < t2, 0 si son iguales, 1 si t1 > t2
int compareTimes(Time* t1, Time* t2) {
    if (t1->hour < t2->hour) return -1;
    if (t1->hour > t2->hour) return 1;
    if (t1->minute < t2->minute) return -1;
    if (t1->minute > t2->minute) return 1;
    return 0;
}

// Compara dos fechas
int compareDates(Date* d1, Date* d2) {
    if (d1->year < d2->year) return -1;
    if (d1->year > d2->year) return 1;
    if (d1->month < d2->month) return -1;
    if (d1->month > d2->month) return 1;
    if (d1->day < d2->day) return -1;
    if (d1->day > d2->day) return 1;
    return 0;
}

// Compara dos DateTime
int compareDateTimes(DateTime* dt1, DateTime* dt2) {
    int date_cmp = compareDates(&dt1->date, &dt2->date);
    if (date_cmp != 0) return date_cmp;
    return compareTimes(&dt1->time, &dt2->time);
}

// Agrega minutos a un Time
void addMinutesToTime(Time* time, int minutes) {
    time->minute += minutes;
    time->hour += time->minute / 60;
    time->minute %= 60;
    time->hour %= 24; // Si querés manejar overflow diario
}

// Devuelve true si target ∈ [start, end]
bool isWithinRange(DateTime* target, DateTime* start, DateTime* end) {
    return compareDateTimes(target, start) >= 0 && compareDateTimes(target, end) <= 0;
}