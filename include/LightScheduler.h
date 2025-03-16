#ifndef LIGHT_SCHEDULER_H
#define LIGHT_SCHEDULER_H
#include "LightControlSpy.h"
#include "TimeService.h"
#include <stdbool.h>
void LightScheduler_init(void);
void LightScheduler_destroy(void);
int LightScheduler_schedule(int lightId, WeekDay day, int minute, int action);
void LightScheduler_remove(int id);
bool matches_day(WeekDay scheduled, WeekDay current) ;
void LightScheduler_wakeup(void) ;
int turn_on_led_now(int id);
int turn_off_led_now(int id);
typedef enum { TURN_OFF, TURN_ON } Action;

typedef struct {
    int id;
    int lightId;
    WeekDay day;
    int minute;
    Action action;
    bool active;
    bool one_minute_befores;
} ScheduledEvent;
#endif
