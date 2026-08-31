#include "state_machine.h"
#include "config.h"
#include <Arduino.h>

StateMachine::StateMachine() : 
    currentState(STATE_NORMAL), 
    previousState(STATE_NORMAL),
    voltageBeforeDrop(220.0),
    timeInWarningState(0) 
{}

bool StateMachine::evaluate(float v_rms) {
    SystemState newState = currentState;

    if (v_rms < VOLTAGE_OUTAGE_MAX) {
        if (currentState != STATE_OUTAGE) {
            newState = STATE_OUTAGE;
        }
    } 
    else if (v_rms < (VOLTAGE_NORMAL_MIN * (1.0 - VOLTAGE_WARNING_DROP / 100.0))) {
        if (currentState == STATE_NORMAL || currentState == STATE_RESTORED) {
            newState = STATE_WARNING;
            voltageBeforeDrop = v_rms; // Record voltage just before it might go to 0
        }
    } 
    else if (v_rms >= VOLTAGE_NORMAL_MIN) {
        if (currentState == STATE_OUTAGE) {
            newState = STATE_RESTORED;
        } else if (currentState == STATE_WARNING) {
            newState = STATE_NORMAL;
        } else {
            newState = STATE_NORMAL;
        }
    }

    if (newState != currentState) {
        previousState = currentState;
        currentState = newState;
        return true; // State changed
    }

    return false; // No change
}

SystemState StateMachine::getCurrentState() const {
    return currentState;
}

SystemState StateMachine::getPreviousState() const {
    return previousState;
}

float StateMachine::getVoltageBeforeDrop() const {
    return voltageBeforeDrop;
}
