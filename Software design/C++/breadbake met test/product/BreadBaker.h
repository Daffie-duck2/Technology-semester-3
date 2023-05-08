#ifndef BREADBAKER_H
#define BREADBAKER_H

#include "Events.h"
#include "interfaces/IDisplay.h"
#include "interfaces/IEventGenerator.h"
#include "interfaces/IExtraIngredientsTray.h"
#include "interfaces/IKneadMotor.h"
#include "interfaces/IOven.h"
#include "interfaces/IStartButtonLed.h"
#include "interfaces/ITimer.h"
#include "interfaces/IYeastTray.h"
#include "Log.h"
#include "States.h"
#include "ProgramStates.h"

class BreadBaker
{
public:
    BreadBaker(IOven& oven, ITimer& timer, IKneadMotor& motor,
               IYeastTray& yeast, IExtraIngredientsTray& extras,
               IDisplay& display, IStartButtonLed& startButton,
               IEventGenerator& eventGenerator, Log& log);

    BreadBaker(const BreadBaker& other) = delete;
    BreadBaker& operator=(const BreadBaker&) = delete;

    bool Pulse();

    // For testing purposes
    // You'll have to decide yourself if you'd rather:
    // - have these methods private (better encapsulation)
    // - be able to test these methods
    // You cannot have both at the same time (without dirty tricks)
    States HandleEvent(Events ev);
    States HandleIdleState(Events ev);
    States HandleStandbyState(Events ev);
    States HandleProcessingState(Events ev);

    Substate HandleSubstateEvent(Events ev);
    Substate HandleRestingState(Events ev);
    Substate HandleKneadingState(Events ev);
    Substate HandleRiseState(Events ev);
    Substate HandleBakingState(Events ev);

    int iteration;
    int number=0;
    

private:
    IOven& oven;
    ITimer& timer;
    IKneadMotor& motor;
    IYeastTray& yeast;
    IExtraIngredientsTray& extras;
    IDisplay& display;
    IStartButtonLed& startButton;
    IEventGenerator& eventGenerator;
    Log& log;
    States state;
    Substate substate;

    void IdleStateEntry();
    void IdleStateExit();
    void StandbyStateEntry();
    void StandbyStateExit();
    void RestingStateEntry();
    void RestingStateExit();
    void KneadingStateEntry(int iteration);
    //void KneadingStateDo(int iteration);
    void KneadingStateExit();
    void RiseStateEntry();
    void RiseStateExit();
    void BakingStateEntry();
    void BakingStateExit();
    void ProcessingStateEntry();
    void ProcessingStateExit();

    void MainStatesEntry();
    void MainStatesExit();
    void SubStatesEntry();
    void SubStatesExit();

    void AnySubstateExit();

    int getProgramtime(ProgramStateName);
    void SwitchMotorRotation(int iteration);
};

#endif
