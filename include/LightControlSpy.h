#ifndef LIGHT_CONTROL_SPY_H 
#define LIGHT_CONTROL_SPY_H 

#include "LightControl.h" 

enum { 
    LIGHT_ID_UNKNOWN = -1, 
    LIGHT_STATE_UNKNOWN = -1, 
    LIGHT_OFF = 0, 
    LIGHT_ON = 1 
}; 

int LightControlSpy_getLastLightId(void); 
int LightControlSpy_getLastState(void); 

#endif