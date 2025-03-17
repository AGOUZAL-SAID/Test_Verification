#include "LightControlSpy.h" 
#include "unity.h" 

// Test case for verifying LightControlSpy functionality
void testLightControlSpyReturnsLastStateChange(void) 
{ 
    // Test setup - initialize light control system
    LightControl_init();
    
    // Verify initial state after initialization
    TEST_ASSERT_EQUAL( LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId() );
    TEST_ASSERT_EQUAL( LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState() ); 
    
    // Test light ON operation and verify state recording
    LightControl_on(42); 
    TEST_ASSERT_EQUAL( 42, LightControlSpy_getLastLightId() ); 
    TEST_ASSERT_EQUAL( LIGHT_ON, LightControlSpy_getLastState() ); 

    // Test cleanup and verify state reset
    LightControl_destroy(); 
    TEST_ASSERT_EQUAL( LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId()); 
    TEST_ASSERT_EQUAL( LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState());
}
