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