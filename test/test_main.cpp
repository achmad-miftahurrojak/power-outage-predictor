#include <Arduino.h>
#include <unity.h>
#include "state_machine.h"
#include "config.h"

StateMachine sm;

void setUp(void) {
    // set stuff up here
    sm = StateMachine(); // Reset state machine
}

void tearDown(void) {
    // clean stuff up here
}

void test_normal_state(void) {
    bool changed = sm.evaluate(220.0);
    TEST_ASSERT_EQUAL(STATE_NORMAL, sm.getCurrentState());
    TEST_ASSERT_FALSE(changed); // Should not change on first init if already normal
}

void test_warning_state(void) {
    sm.evaluate(220.0); // Normal
    
    // Drop by 25% (220 * 0.75 = 165), threshold is 20% (160) from 200V.
    // VOLTAGE_NORMAL_MIN is 200.0, WARNING drop is 20%. Threshold = 200 * 0.8 = 160V.
    bool changed = sm.evaluate(150.0);
    
    TEST_ASSERT_EQUAL(STATE_WARNING, sm.getCurrentState());
    TEST_ASSERT_TRUE(changed);
    TEST_ASSERT_EQUAL_FLOAT(150.0, sm.getVoltageBeforeDrop());
}

void test_outage_state(void) {
    sm.evaluate(220.0); // Normal
    sm.evaluate(150.0); // Warning
    
    // Drop below outage threshold (10.0V)
    bool changed = sm.evaluate(5.0);
    
    TEST_ASSERT_EQUAL(STATE_OUTAGE, sm.getCurrentState());
    TEST_ASSERT_TRUE(changed);
}

void test_restored_state(void) {
    sm.evaluate(220.0); // Normal
    sm.evaluate(150.0); // Warning
    sm.evaluate(0.0);   // Outage
    
    // Restore to normal
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
