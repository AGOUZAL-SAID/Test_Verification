#include "LightControlSpy.h"
#include <stdio.h>

static int id_test = -1;
static int state_test = -1; 
int LightControlSpy_getLastLightId(){
    return id_test;
} 

int LightControlSpy_getLastState(){
    return state_test ;
}

void LightControl_init(){
    id_test    = LIGHT_ID_UNKNOWN;
    state_test = LIGHT_STATE_UNKNOWN;
}
void LightControl_destroy(){
    printf("light control destroyed");
    state_test = LIGHT_STATE_UNKNOWN ;
    id_test    = LIGHT_ID_UNKNOWN;
}
void LightControl_on(int id){
    id_test = id;
    state_test = LIGHT_ON;
}
void LightControl_off(int id){
    state_test = LIGHT_OFF;
    id_test = id;
}

