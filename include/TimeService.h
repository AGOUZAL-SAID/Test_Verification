#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

typedef enum {
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
} WeekDay;

typedef struct {
    WeekDay dayOfWeek;
    int minuteOfDay;
} Time;

/* Initialize time service */
void TimeService_init(void);

/* Finalize time service */
void TimeService_destroy(void);

/* Modifies the passed struct to reflect the current time */
void TimeService_getTime(Time *time);

/* Initialize periodic alarm. The function callback will be called by the time
   service every seconds seconds. The function returns a handle on the alarm. */
int  TimeService_startPeriodicAlarm(int seconds, void (*callback)(void));

/* Stops periodic alarm corresponding to the handle */
void TimeService_stopPeriodicAlarm(int handle);

#endif
