#include "LightControlSpy.h" 
#include "unity.h" 

void testLightControlSpyReturnsLastStateChange(void) 
{ 
    LightControl_init();
    TEST_ASSERT_EQUAL( LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId() );
    TEST_ASSERT_EQUAL( LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState() ); 
    
    LightControl_on(42); 
    TEST_ASSERT_EQUAL( 42, LightControlSpy_getLastLightId() ); 
    TEST_ASSERT_EQUAL( LIGHT_ON, LightControlSpy_getLastState() ); 

    LightControl_destroy(); 
    TEST_ASSERT_EQUAL( LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId()); 
    TEST_ASSERT_EQUAL( LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState());
}