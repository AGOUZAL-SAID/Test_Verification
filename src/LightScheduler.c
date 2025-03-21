#include "LightScheduler.h"
#include "LightControl.h"
#include "TimeService.h"
#include <stdlib.h>
#include <stdbool.h>

// Global variables for storing scheduled events
static ScheduledEvent events[256];  // Stores up to 256 scheduled light events
static int eventCount = 0;          // Tracks number of active scheduled events

// Initialize light scheduler - reset event count and mark all events inactive
void LightScheduler_init(void) {
    LightControl_init();
    eventCount = 0;
    for(int i = 0; i < 256; i++) {
        events[i].active = false;  // Initialize all event slots as inactive
    }
    TimeService_startPeriodicAlarm(60,LightScheduler_wakeup);
}

// Cleanup function (currently empty)
void LightScheduler_destroy(void) {
    LightControl_destroy();
}

// Schedule a new light event with validation
int LightScheduler_schedule(int lightId, WeekDay day, int minute, int action) {
    // Validate light ID range and check event capacity
    if(lightId < 0 || lightId > 255 || eventCount >= 256) return -1;
    if(minute<0 || minute>23*60+59)return -1;
    // Create new event and add to array
    events[eventCount] = (ScheduledEvent){
        .id = eventCount,          // Auto-increment ID based on position
        .lightId = lightId,        // Target light ID (0-255)
        .day = day,                // Scheduled day/week pattern
        .minute = minute,          // Scheduled time in minutes
        .action = action,          // TURN_ON (1) or TURN_OFF (0)
        .active = true,            // Mark event as active
        .one_minute_befores = true // Currently unused legacy flag
    };
    return eventCount++;  // Return event ID and increment counter
}

// Remove/deactivate an event by ID
void LightScheduler_remove(int id) {
    if(id >= 0 && id < 256) events[id].active = false;  // Soft delete by deactivation
}

// Day matching logic for different schedule types
bool matches_day(WeekDay scheduled, WeekDay current) {
    if(scheduled == EVERYDAY) return true;  // Match any day
    if(scheduled == WEEKDAY) return current >= MONDAY && current <= FRIDAY;  // Weekdays
    if(scheduled == WEEKEND) return current == SATURDAY || current == SUNDAY;  // Weekends
    return scheduled == current;  // Direct day match
}

// Main scheduler loop - checks and triggers events
void LightScheduler_wakeup(void) {
    Time timeNow;
    TimeService_getTime(&timeNow);  // Get current time
    
    // Check all scheduled events
    for(int i = 0; i < eventCount; i++) {
        ScheduledEvent *e = &events[i];
        // Check if event is active and day matches
        if(e->active && matches_day(e->day, timeNow.dayOfWeek)) {
            // Execute action if time matches exactly
            if(e->minute - timeNow.minuteOfDay == 0) {
                (e->action == TURN_ON) ? LightControl_on(e->lightId) 
                                       : LightControl_off(e->lightId);
            }
        }
    }
}

// Immediate light control with validation
int turn_on_led_now(int id){
    if (id < 0 || id > 255 ) return -1;  // Validate ID
    LightControl_on(id);                 // Direct control
    return 0;
}

int turn_off_led_now(int id){
    if (id < 0 || id > 255 ) return -1;  // Validate ID
    LightControl_off(id);                // Direct control
    return 0;
}

// Legacy flag check (currently always returns true)
bool did_u_wake_me_up_one_minute_before(int id){
    return events[id].one_minute_befores;  // Return preset flag value
}