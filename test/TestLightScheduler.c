#include "MockTimeService.h"
#include "LightScheduler.h"
#include "unity.h"
#include <stdbool.h>

// Mocks
static Time currentTime; // Holds the mocked current time for testing

// Test setup and teardown
void setUp(void) {
    LightScheduler_init();    // Initialize the LightScheduler before each test
    LightControl_init();      // Initialize the LightControl module
    currentTime = (Time){NONE, -1}; // Reset mocked time to default invalid values
}

void tearDown(void) {
    LightScheduler_destroy(); // Clean up LightScheduler after each test
}

// Helper function to set mock time
static void set_time(WeekDay day, int minute) {
    currentTime.dayOfWeek = day;
    currentTime.minuteOfDay = minute;
}

// Test cases
// Tests scheduling a light to turn ON on a specific day and time
void test_schedule_light_on_specific_day_time(void) {
    LightScheduler_init();
    LightScheduler_schedule(1, MONDAY, 8*60, TURN_ON);
    turn_off_led_now(1);
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
}

// Tests scheduling a light to turn OFF on a specific day and time
void test_schedule_light_off_specific_day_time(void) {
    LightScheduler_init();
    LightScheduler_schedule(2, FRIDAY, 22*60, TURN_OFF);
    turn_on_led_now(2);
    set_time(FRIDAY, 22*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Tests daily schedule (EVERYDAY) functionality
void test_everyday_schedule(void) {
    LightScheduler_init();
    LightScheduler_schedule(3, EVERYDAY, 12*60, TURN_ON);
    turn_off_led_now(3);
    set_time(TUESDAY, 12*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(3, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    turn_off_led_now(3);
    set_time(SUNDAY, 12*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(3, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
}

// Tests weekday schedule (Monday-Friday) functionality
void test_weekday_schedule(void) {
    LightScheduler_init();
    LightScheduler_schedule(3, WEEKDAY, 12*60, TURN_ON);
    set_time(SUNDAY, 12*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,LightControlSpy_getLastState());
    turn_off_led_now(3);
    set_time(MONDAY, 12*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(3, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
}

// Tests weekend schedule (Saturday-Sunday) functionality
void test_weekend_schedule(void) {
    LightScheduler_init();
    LightScheduler_schedule(4, WEEKEND, 9*60, TURN_ON);
    turn_off_led_now(4);
    set_time(SATURDAY, 9*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    turn_off_led_now(4);
    set_time(SUNDAY, 9*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
}

// Tests removal of scheduled events
void test_remove_scheduled_event(void) {
    LightScheduler_init();
    int id = LightScheduler_schedule(6, TUESDAY, 7*60, TURN_ON);
    LightScheduler_remove(id);
    set_time(TUESDAY, 7*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_STATE_UNKNOWN,LightControlSpy_getLastState());
}

// Tests maximum number of scheduled events
void test_max_events(void) {
    LightScheduler_init();
    for(int i = 0; i < 256; i++) {
        TEST_ASSERT_NOT_EQUAL(-1, LightScheduler_schedule(i, MONDAY, 0, TURN_ON));
    }
    TEST_ASSERT_EQUAL(-1, LightScheduler_schedule(256, MONDAY, 0, TURN_ON));
}

// Tests daily schedule for turning OFF lights
void test_everyday_schedule_off(void) {
    LightScheduler_init();
    LightScheduler_schedule(3, EVERYDAY, 12*60, TURN_OFF);
    turn_on_led_now(3);
    set_time(TUESDAY, 12*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(3, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Tests weekday schedule for turning OFF lights
void test_weekday_schedule_off(void) {
    LightScheduler_init();
    LightScheduler_schedule(3, WEEKDAY, 12*60, TURN_OFF);
    set_time(MONDAY, 12*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(3, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Tests weekend schedule for turning OFF lights
void test_weekend_schedule_off(void) {
    LightScheduler_init();
    LightScheduler_schedule(4, WEEKEND, 9*60, TURN_OFF);
    turn_on_led_now(4);
    set_time(SATURDAY, 9*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
    turn_on_led_now(4);
    set_time(SUNDAY, 9*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Tests immediate LED turn OFF functionality
void test_scheduler_can_off_led(void){
    turn_off_led_now(80);
    TEST_ASSERT_EQUAL(80, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Tests immediate LED turn ON functionality
void test_scheduler_can_on_led(void){
    turn_on_led_now(80);
    TEST_ASSERT_EQUAL(80, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
}

// Tests handling of invalid LED IDs for turn ON operations
void test_on_invalid_led(void){
    int i = turn_on_led_now(300);
    TEST_ASSERT_EQUAL(-1, i);
    i = turn_on_led_now(-2);
    TEST_ASSERT_EQUAL(-1, i);
}

// Tests handling of invalid LED IDs for turn OFF operations
void test_off_invalid_led(void){
    int i = turn_off_led_now(300);
    TEST_ASSERT_EQUAL(-1, i);
    i = turn_off_led_now(-2);
    TEST_ASSERT_EQUAL(-1, i);
}

// Tests scheduling with invalid LED ID for ON operation
void test_schedule_light_on_specific_day_time_invalidLED(void) {
    LightScheduler_init();
    int i = LightScheduler_schedule(300, MONDAY, 8*60, TURN_ON);
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(-1, i);
}

// Tests scheduling with invalid LED ID for OFF operation
void test_schedule_light_off_specific_day_time_invalidLED(void) {
    LightScheduler_init();
    int i = LightScheduler_schedule(300, MONDAY, 8*60, TURN_OFF);
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(-1, i);
}

// Tests multiple scheduled events on a single LED
void test_schedule_light_on_multiple_event_oneLED(void){
    LightScheduler_init();
    LightScheduler_schedule(55, MONDAY, 8*60, TURN_ON);
    LightScheduler_schedule(55, FRIDAY, 5*60, TURN_ON);
    LightScheduler_schedule(55, TUESDAY, 7*30, TURN_ON);
    LightScheduler_schedule(55, SUNDAY, 7*30, TURN_OFF);
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(55, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    turn_off_led_now(55);
    set_time( FRIDAY, 5*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(55, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    turn_off_led_now(55);
    set_time( TUESDAY, 7*30);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(55, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    turn_on_led_now(55);
    set_time( SUNDAY, 7*30);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(55, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Tests integration with external driver system
void test_shcheduler_passed_by_driver(){
    LightScheduler_init();
    LightScheduler_schedule(1, MONDAY, 8*60, TURN_ON);
    turn_off_led_now(1);
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    TEST_ASSERT_TRUE(did_you_pass_by_me());
}

// Tests wakeup functionality one minute before scheduled time
void test_scheduler_weekup_one_minute_before(){
    LightScheduler_init();
    int id = LightScheduler_schedule(1, MONDAY, 8*60, TURN_ON);
    turn_off_led_now(1);
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    TEST_ASSERT_TRUE(did_u_wake_me_up_one_minute_before(id));
}


// Test to verify that the scheduler turns on the LED 
// only at the specified time (Monday 8:00) and keeps it off otherwise.
// It iterates through all days/times except the scheduled time to check the LED remains off,
// then checks the scheduled time turns it on.
void test_scheduler_turn_on_led_only_at_the_specifique_time(){
    LightScheduler_init();
    LightScheduler_schedule(1, MONDAY, 8*60, TURN_ON);
    turn_off_led_now(1);
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,SATURDAY,FRIDAY,SUNDAY,WEDNESDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    if(day ==MONDAY && hour==8 && minute==0 ){continue;}
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
                }
            }
    }
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
}


// Test to ensure the LED turns on only during weekend days (Saturday/Sunday) 
// at 4:30. Tests all other times/days to confirm the LED stays off,
// then validates both weekend days at the scheduled time.
void test_scheduler_turn_on_led_only_at_the_weekend(){
    LightScheduler_init();
    LightScheduler_schedule(1, WEEKEND, 4*60+30, TURN_ON);
    turn_off_led_now(1);
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,SATURDAY,FRIDAY,SUNDAY,WEDNESDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    if((day ==SATURDAY || day==SUNDAY) && hour==4 && minute==30 ){continue;}
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
                }
            }
    }
    set_time(SUNDAY, 4*60+30);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    turn_off_led_now(1);
    set_time(SATURDAY, 4*60+30);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());

}

// Test to validate the LED turns on only on weekdays (Monday-Friday) at 18:45.
// Loops through all non-weekday times to check the LED remains off,
// then verifies each weekday at the scheduled time.
void test_scheduler_turn_on_led_only_at_weekday(){
    LightScheduler_init();
    LightScheduler_schedule(1, WEEKDAY, 18*60+45, TURN_ON);
    turn_off_led_now(1);
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,WEDNESDAY,FRIDAY,SUNDAY,SATURDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    if(day >= MONDAY && day <= FRIDAY && hour==18 && minute==45){continue;}
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
                }
            }
    }
    for(int i =0 ; i<5 ; i++){
        WeekDay day = week[i];
        turn_off_led_now(1);
        set_time(day,18*60+45);
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
        LightScheduler_wakeup();
        TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
        TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
    }
}

// Test to verify the scheduler turns off the LED 
// only at the specified time (Monday 8:00). Checks all other times/days 
// to ensure the LED remains on, then tests the scheduled turn-off time.
void test_scheduler_turn_off_led_only_at_the_specifique_time(){
    LightScheduler_init();
    LightScheduler_schedule(1, MONDAY, 8*60, TURN_OFF);
    turn_on_led_now(1);
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,SATURDAY,FRIDAY,SUNDAY,WEDNESDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    if(day ==MONDAY && hour==8 && minute==0 ){continue;}
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
                }
            }
    }
    set_time(MONDAY, 8*60);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
}

// Test to ensure the LED turns off only during weekends (Saturday/Sunday) at 4:30.
// Validates all other times/days keep the LED on, then checks both weekend days.
void test_scheduler_turn_off_led_only_at_the_weekend(){
    LightScheduler_init();
    LightScheduler_schedule(1, WEEKEND, 4*60+30, TURN_OFF);
    turn_on_led_now(1);
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,SATURDAY,FRIDAY,SUNDAY,WEDNESDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    if((day ==SATURDAY || day==SUNDAY) && hour==4 && minute==30 ){continue;}
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON,LightControlSpy_getLastState(),"dans la boucle");
                }
            }
    }
    set_time(SUNDAY, 4*60+30);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
    turn_on_led_now(1);
    set_time(SATURDAY, 4*60+30);
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
    LightScheduler_wakeup();
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());

}

// Test to confirm the LED turns off only on weekdays (Monday-Friday) at 18:45.
// Tests non-weekday times to ensure the LED stays on,
// then checks each weekday at the scheduled time.
void test_scheduler_turn_off_led_only_at_weekday(){
    LightScheduler_init();
    LightScheduler_schedule(1, WEEKDAY, 18*60+45, TURN_OFF);
    turn_on_led_now(1);
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,WEDNESDAY,FRIDAY,SUNDAY,SATURDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    if(day >= MONDAY && day <= FRIDAY && hour==18 && minute==45){continue;}
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
                }
            }
    }
    for(int i =0 ; i<5 ; i++){
        WeekDay day = week[i];
        turn_on_led_now(1);
        set_time(day,18*60+45);
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
        LightScheduler_wakeup();
        TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
        TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
    }
}

// Comprehensive test to validate the scheduler can correctly program a TURN_ON event
// for *any* valid time (all days, all hours, all minutes). Reinitializes the scheduler
// for each iteration to avoid state leakage.
void test_scheduler_can_programme_on_any_time_possible(){
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,WEDNESDAY,FRIDAY,SUNDAY,SATURDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    LightScheduler_init();
                    LightScheduler_schedule(200, day, hour*60+minute, TURN_ON);
                    turn_off_led_now(200);
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(200, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_ON,LightControlSpy_getLastState());
                }
            }
    }
}

// Comprehensive test to validate the scheduler can correctly program a TURN_OFF event
// for *any* valid time (all days, all hours, all minutes). Reinitializes the scheduler
// for each iteration to ensure test isolation.
void test_scheduler_can_programme_off_any_time_possible(){
    WeekDay  week[7] = {MONDAY,TUESDAY,THURDSDAY,WEDNESDAY,FRIDAY,SUNDAY,SATURDAY};
    for(int i =0 ;i<7;i++){
        WeekDay day = week[i];
            for(int hour =0 ; hour<24;hour++){
                for(int minute = 0 ; minute < 60 ; minute++){
                    LightScheduler_init();
                    LightScheduler_schedule(200, day, hour*60+minute, TURN_OFF);
                    turn_on_led_now(200);
                    set_time(day, hour*60+minute);
                    TimeService_getTime_ExpectAnyArgs();
                    TimeService_getTime_ReturnMemThruPtr_time(&currentTime,sizeof(currentTime));
                    LightScheduler_wakeup();
                    TEST_ASSERT_EQUAL(200, LightControlSpy_getLastLightId());
                    TEST_ASSERT_EQUAL(LIGHT_OFF,LightControlSpy_getLastState());
                }
            }
    }
}