#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

enum Day {
    NONE=-1,
    MONDAY=1,
    TUESDAY,
    WEDNESDAY,
    THURDSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY,
    EVERYDAY=10,
    WEEKDAY,
    WEEKEND
};

typedef enum Day Day;

typedef struct {
    Day day;
    int minuteOfDay;
} Time;

void TimeService_init(void);
void TimeService_destroy(void);
void TimeService_getTime(Time *time);
void TimeService_startPeriodicAlarm(int seconds, void (*callback)(void));
void TimeService_stopPeriodicAlarm(int seconds);

#endif
