#include <Arduino.h>
#include <unity.h>
#include "state_machine.h"
#include "config.h"

StateMachine sm;

void setUp(void) {
    
    sm = StateMachine(); 
}

void tearDown(void) {
    
}

void test_normal_state(void) {
    bool changed = sm.evaluate(220.0);
    TEST_ASSERT_EQUAL(STATE_NORMAL, sm.getCurrentState());
    TEST_ASSERT_FALSE(changed); 
}

void test_warning_state(void) {
    sm.evaluate(220.0); 
    
    
    
    bool changed = sm.evaluate(150.0);
    
    TEST_ASSERT_EQUAL(STATE_WARNING, sm.getCurrentState());
    TEST_ASSERT_TRUE(changed);
    TEST_ASSERT_EQUAL_FLOAT(150.0, sm.getVoltageBeforeDrop());
}

void test_outage_state(void) {
    sm.evaluate(220.0); 
    sm.evaluate(150.0); 
    
    
    bool changed = sm.evaluate(5.0);
    
    TEST_ASSERT_EQUAL(STATE_OUTAGE, sm.getCurrentState());
    TEST_ASSERT_TRUE(changed);
}

void test_restored_state(void) {
    sm.evaluate(220.0); 
    sm.evaluate(150.0); 
    sm.evaluate(0.0);   
    
    
    bool changed = sm.evaluate(210.0);
    
    TEST_ASSERT_EQUAL(STATE_RESTORED, sm.getCurrentState());
    TEST_ASSERT_TRUE(changed);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_normal_state);
    RUN_TEST(test_warning_state);
    RUN_TEST(test_outage_state);
    RUN_TEST(test_restored_state);
    UNITY_END();
    
    return 0;
}
