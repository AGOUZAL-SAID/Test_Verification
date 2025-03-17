#include "LightControlSpy.h"
#include <stdio.h>

// Spy variables for tracking light control states
static int id_test = -1;        // Stores last operated light ID (LIGHT_ID_UNKNOWN = -1)
static int state_test = -1;     // Stores last light operation state (LIGHT_STATE_UNKNOWN = -1)
static int passed_by_me = 0;    // Flag for driver integration testing verification

// Get last operated light ID from spy
int LightControlSpy_getLastLightId(){
    return id_test;
} 

// Get last light operation state from spy
int LightControlSpy_getLastState(){
    return state_test;
}

// Reset spy tracking for new test
void LightControl_init(){
    id_test    = LIGHT_ID_UNKNOWN;      // Reset to default unknown ID
    state_test = LIGHT_STATE_UNKNOWN;   // Reset to default unknown state
    passed_by_me = 1;                   // Set verification flag for initialization
}

// Cleanup spy and print destruction message
void LightControl_destroy(){
    printf("light control destroyed");
    state_test = LIGHT_STATE_UNKNOWN;   // Reset state tracking
    id_test    = LIGHT_ID_UNKNOWN;      // Reset ID tracking
    passed_by_me = 1;                   // Set verification flag for destruction
}

// Spy implementation of light ON operation
void LightControl_on(int id){
    id_test = id;              // Record light ID
    state_test = LIGHT_ON;     // Record ON operation
}

// Spy implementation of light OFF operation 
void LightControl_off(int id){
    state_test = LIGHT_OFF;    // Record OFF operation
    id_test = id;              // Record light ID
    passed_by_me = 1;          // Set verification flag for driver integration
}

// Check if control path was executed (for driver integration tests)
int did_you_pass_by_me(){
    return passed_by_me==1;    // Return true if any control operation occurred
}