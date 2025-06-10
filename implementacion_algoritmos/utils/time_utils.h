#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <stdbool.h>

typedef struct Time {
    int hour;
    int minute;
} Time;

typedef struct Date {
    int day;
    int month;
    int year;
} Date;

typedef struct DateTime {
    Date date;
    Time time;
} DateTime;

// Constructores
Time* newTime(int hour, int minute);
Date* newDate(int day, int month, int year);
DateTime* newDateTime(Date* date, Time* time);

// Funciones auxiliares
int compareTimes(Time* t1, Time* t2);
int compareDates(Date* d1, Date* d2);
int compareDateTimes(DateTime* dt1, DateTime* dt2);
void addMinutesToTime(Time* time, int minutes);
bool isWithinRange(DateTime* target, DateTime* start, DateTime* end);

#endif
