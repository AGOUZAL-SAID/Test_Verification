#include "LightControlSpy.h"
#include <stdio.h>
static int id_test = -1;
static int state_test = -1; 
static int passed_by_me = 0;
int LightControlSpy_getLastLightId(){
    return id_test;
} 

int LightControlSpy_getLastState(){
    return state_test ;
}

void LightControl_init(){
    id_test    = LIGHT_ID_UNKNOWN;
    state_test = LIGHT_STATE_UNKNOWN;
    passed_by_me = 1;
}

void LightControl_destroy(){
    printf("light control destroyed");
    state_test = LIGHT_STATE_UNKNOWN ;
    id_test    = LIGHT_ID_UNKNOWN;
    passed_by_me = 1;
}
void LightControl_on(int id){
    id_test = id;
    state_test = LIGHT_ON;
    passed_by_me = 1;
}

void LightControl_off(int id){
    state_test = LIGHT_OFF;
    id_test = id;
    passed_by_me = 1;
}
int did_you_pass_by_me(){return passed_by_me==1; }

