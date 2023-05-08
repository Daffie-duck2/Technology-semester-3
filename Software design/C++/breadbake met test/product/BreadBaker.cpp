#include "BreadBaker.h"
#include "Program.h"
#include "TimeConstants.h"

BreadBaker::BreadBaker(
    IOven &oven, ITimer &timer, IKneadMotor &motor, IYeastTray &yeast,
    IExtraIngredientsTray &extras, IDisplay &display,
    IStartButtonLed &startButton, IEventGenerator &eventGenerator,
    Log &log)
    : oven(oven), timer(timer), motor(motor), yeast(yeast), extras(extras), display(display), startButton(startButton), eventGenerator(eventGenerator), log(log), state(States::STANDBY), substate(Substate::RESTING)
{
    MainStatesEntry();
}

bool BreadBaker::Pulse()
{
    auto ev = eventGenerator.GetEvent();
    if (ev != std::nullopt)
    {
        HandleEvent(*ev);
    }
    return ev != std::nullopt;
}

int BreadBaker::getProgramtime(ProgramStateName name) //struct!
{
    int time = 0;
    switch(number)
    {
    case 0:
    case 1:
        switch (name)
        {
        case ProgramStateName::resting:
            time = 60;
            break;
        case ProgramStateName::kneading:
            time = 20;
            break;
        case ProgramStateName::rise:
            time = 160;
            break;
        case ProgramStateName::baking:
            time = 50;
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (name)
        {
        case ProgramStateName::resting:
            time = 0;
            break;
        case ProgramStateName::kneading:
            time = 15;
            break;
        case ProgramStateName::rise:
            time = 60;
            break;
        case ProgramStateName::baking:
            time = 40;
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (name)
        {
        case ProgramStateName::resting:
            time = 40;
            break;
        case ProgramStateName::kneading:
            time = 20;
            break;
        case ProgramStateName::rise:
            time = 80;
            break;
        case ProgramStateName::baking:
            time = 0;
            break;
        default:
            break;
        }
        break;
    case 4:
        switch (name)
        {
        case ProgramStateName::resting:
            time = 0;
            break;
        case ProgramStateName::kneading:
            time = 0;
            break;
        case ProgramStateName::rise:
            time = 0;
            break;
        case ProgramStateName::baking:
            time = 30;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return time;
}

void BreadBaker::SwitchMotorRotation(int iteration)
{
    if((iteration % 2) == 0)
    {
        motor.TurnLeft();
    }
    else
    {
        motor.TurnRight();
    }
}

States BreadBaker::HandleEvent(Events ev)
{
    switch (state)
    {
    case States::IDLE:
        state = HandleIdleState(ev);
        break;
    case States::STANDBY:
        state = HandleStandbyState(ev);
        break;
    case States::PROCESSING:
        state = HandleProcessingState(ev);
        break;
    default:
        //error
        break;
    };
    return state;
}

Substate BreadBaker::HandleSubstateEvent(Events ev)
{
    //Substate substate = Substate::RESTING;

    switch(substate)
    {
    case Substate::RESTING:
        substate = HandleRestingState(ev);
        break;
    case Substate::KNEADING:
        substate = HandleKneadingState(ev);
        break;
    case Substate::RISE:
        substate = HandleRiseState(ev);
        break;
    case Substate::BAKING:
        substate = HandleBakingState(ev);
        break;
    default:
        //error
        break;
    }
    return substate;
}

States BreadBaker::HandleIdleState(Events ev)
{
    States result = States::IDLE;

    switch(ev)
    {
    case Events::MENU_BUTTON_LONG_PRESSED:
        IdleStateExit();

        result = States::STANDBY;

        StandbyStateEntry();
        break;

    case Events::TIMER_TIMEOUT:
        IdleStateExit();

        result = States::STANDBY;

        StandbyStateEntry();
        break;
    
    case Events::MENU_BUTTON_PRESSED:
        IdleStateExit();

        number = (number+1)%5;
        display.SetMenu(number);
        result = States::IDLE;

        IdleStateEntry();
        break;
    
    case Events::START_BUTTON_PRESSED:
        IdleStateExit();

        result = States::PROCESSING;
        substate = Substate::RESTING;

        if(oven.GetTemperature() < 50)
        {
            result = States::PROCESSING;
            ProcessingStateEntry();
            RestingStateEntry();
        }
        else
        {
            result = States::IDLE;
            IdleStateEntry();
        }

       
        break;
    default:
        //nothing
        break;
    }

    return result;
}

States BreadBaker::HandleProcessingState(Events ev)
{

    States state = States::IDLE;
    
    switch(ev)
    {
    case Events::MENU_BUTTON_LONG_PRESSED:
        AnySubstateExit();
        ProcessingStateExit();

        state = States::IDLE;

        IdleStateEntry();
        break;

    case Events::OVEN_DONE:
        AnySubstateExit();
        ProcessingStateExit();

        state = States::IDLE;

        IdleStateEntry();
        break;
    
    default:
        substate = HandleSubstateEvent(ev);
        break;
    }

    return state;
}

States BreadBaker::HandleStandbyState(Events ev)
{
    States result = States::STANDBY;

    switch (ev)
    {
    case Events::MENU_BUTTON_PRESSED:
        StandbyStateExit();

        result = States::IDLE;

        IdleStateEntry();
        break;

    default:
        //error
        break;
    }
    
    return result;
}

Substate BreadBaker::HandleRestingState(Events ev)
{
    Substate result = Substate::RESTING;

    switch(ev)
    {
    case Events::TIMER_TIMEOUT:
        RestingStateExit();

        result = Substate::KNEADING;
        iteration = 0;
        yeast.Drop(getProgramtime(ProgramStateName::kneading)/2);
        extras.Drop(getProgramtime(ProgramStateName::kneading)/2);

        KneadingStateEntry(iteration);
        break;
    
    case Events::MENU_BUTTON_LONG_PRESSED:
        AnySubstateExit();
        RestingStateExit();

        //result = States::IDLE;

        IdleStateEntry();
        break;
    
    default:
        //nothing
        break;
    }

    return result;
}

Substate BreadBaker::HandleKneadingState(Events ev)
{
    Substate result = Substate::KNEADING;

    switch(ev)
    {
    case Events::TIMER_TIMEOUT:
        KneadingStateExit();
        if(iteration < getProgramtime(ProgramStateName::kneading))
        {
            iteration++;
            KneadingStateEntry(iteration);
            //KneadingStateDo(iteration);
            result = Substate::KNEADING;
            return result;
        }        
        result = Substate::RISE;
        RiseStateEntry();
        break;
    
    case Events::MENU_BUTTON_LONG_PRESSED:
        KneadingStateExit();
        AnySubstateExit();
        state = States::IDLE;
        break;

    default:
        //nothing
        break;
    }

    return result;
}

Substate BreadBaker::HandleRiseState(Events ev)
{
    Substate result = Substate::RISE;

    switch(ev)
    {
    case Events::OVEN_DONE:
        RiseStateExit();

        result = Substate::BAKING;

        BakingStateEntry();
        break;

    case Events::MENU_BUTTON_LONG_PRESSED:
        RiseStateExit();

        state = States::IDLE;
        IdleStateEntry();
        break;

    default:
        //nothing
        break;
    }
    
    return result;
}

Substate BreadBaker::HandleBakingState(Events ev)
{
    Substate result = Substate::BAKING;

    switch(ev)
    {
    case Events::OVEN_DONE:
        BakingStateExit();
        AnySubstateExit();

        state = States::IDLE;

        IdleStateEntry();
        break;
    
    case Events::MENU_BUTTON_LONG_PRESSED:
        BakingStateExit();
        AnySubstateExit();

        state = States::IDLE;

        IdleStateEntry();
        break;
    default:
        //nothing
        break;
    }
    return result;
}

void BreadBaker::IdleStateEntry()
{
     
}

void BreadBaker::IdleStateExit()
{
}

void BreadBaker::StandbyStateEntry()
{
}

void BreadBaker::StandbyStateExit()
{

}

void BreadBaker::RestingStateEntry()
{
    timer.Set(getProgramtime(ProgramStateName::resting));
}

void BreadBaker::RestingStateExit()
{
}

void BreadBaker::KneadingStateEntry(int iteration)
{
    if(getProgramtime(ProgramStateName::kneading)> 0)
    {
        timer.Set(1);
    }
    else
    {
        timer.Set(0);
        return;
    }
    SwitchMotorRotation(iteration);
}



void BreadBaker::KneadingStateExit()
{
    motor.Stop();
    timer.Cancel();
    yeast.Cancel();
    extras.Cancel();
}

void BreadBaker::RiseStateEntry()
{
    oven.StartRise(getProgramtime(ProgramStateName::rise));
}

void BreadBaker::RiseStateExit()
{
    oven.Cancel();
}

void BreadBaker::BakingStateEntry()
{
    oven.StartBake(getProgramtime(ProgramStateName::baking));
}

void BreadBaker::BakingStateExit()
{
    oven.Cancel();
}

void BreadBaker::MainStatesEntry()
{
}

void BreadBaker::MainStatesExit()
{
}

void BreadBaker::SubStatesEntry()
{
}

void BreadBaker::SubStatesExit()
{
}

void BreadBaker::ProcessingStateEntry()
{
}

void BreadBaker::ProcessingStateExit()
{
}

void BreadBaker::AnySubstateExit()
{
    oven.Cancel();
    timer.Cancel();
    yeast.Cancel();
    extras.Cancel();
    motor.Stop();
    display.SetMenu(0);
}
