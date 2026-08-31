#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

enum SystemState {
    STATE_NORMAL,
    STATE_WARNING,
    STATE_OUTAGE,
    STATE_RESTORED
};

class StateMachine {
public:
    StateMachine();
    
    // Evaluate the new RMS voltage and return true if state changed
    bool evaluate(float v_rms);
    
    SystemState getCurrentState() const;
    SystemState getPreviousState() const;
    
    float getVoltageBeforeDrop() const;
    
private:
    SystemState currentState;
    SystemState previousState;
    float voltageBeforeDrop;
    
    // For hysteresis / debouncing
    unsigned long timeInWarningState;
};

#endif // STATE_MACHINE_H
