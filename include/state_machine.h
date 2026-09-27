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
    
    
    bool evaluate(float v_rms);
    
    SystemState getCurrentState() const;
    SystemState getPreviousState() const;
    
    float getVoltageBeforeDrop() const;
    
private:
    SystemState currentState;
    SystemState previousState;
    float voltageBeforeDrop;
    float lastNormalVoltage;
    
    
    unsigned long timeInWarningState;
};

#endif 
