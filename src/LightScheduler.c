#include "LightScheduler.h"
#include "LightControl.h"
#include "TimeService.h"
#include <stdlib.h>
#include <stdbool.h>


static ScheduledEvent events[256];
static int eventCount = 0;

void LightScheduler_init(void) {
    eventCount = 0;
    for(int i = 0; i < 256; i++) {
        events[i].active = false;
    }
}

void LightScheduler_destroy(void) {
}

int LightScheduler_schedule(int lightId, WeekDay day, int minute, int action) {
    if(lightId < 0 || lightId > 255 || eventCount >= 256) return -1;
    
    events[eventCount] = (ScheduledEvent){
        .id = eventCount,
        .lightId = lightId,
        .day = day,
        .minute = minute,
        .action = action,
        .active = true,
        .one_minute_befores =true
    };
    return eventCount++;
}

void LightScheduler_remove(int id) {
    if(id >= 0 && id < 256) events[id].active = false;
}

bool matches_day(WeekDay scheduled, WeekDay current) {
    if(scheduled == EVERYDAY) return true;
    if(scheduled == WEEKDAY) return current >= MONDAY && current <= FRIDAY;
    if(scheduled == WEEKEND) return current == SATURDAY || current == SUNDAY;
    return scheduled == current;
}

void LightScheduler_wakeup(void) {
    Time timeNow;
    TimeService_getTime(&timeNow);
    for(int i = 0; i < eventCount; i++) {
        ScheduledEvent *e = &events[i];
        if(e->active && matches_day(e->day, timeNow.dayOfWeek)) {
            if(e->minute - timeNow.minuteOfDay == 0) {
                (e->action == TURN_ON) ? LightControl_on(e->lightId) : LightControl_off(e->lightId);
            }
        }
    }
}

int turn_on_led_now(int id){
    if (id < 0 || id > 255 ) return -1;
    LightControl_on(id);
    return 0;}

int turn_off_led_now(int id){
    if (id < 0 || id > 255 ) return -1;
    LightControl_off(id);
    return 0;}

bool did_u_wake_me_up_one_minute_before(int id){
    return events[id].one_minute_befores;
}