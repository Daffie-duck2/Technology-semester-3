#include "StateMachine.h"
#include "Wire.h"
#include <Arduino.h>

StateMachine::StateMachine():state(State::Slave)
{
    
}
StateMachine::~StateMachine()
{
    
}

void StateMachine::EventHandler(Event event)
{
    Serial.print("Help");
    switch (state)
    {
        case Dominant:
        {
            state = HandleDominantState(event);
            break;
        }
        case Slave:
        {
            state = HandleSlaveState(event);
            break;
        }
        case RequestingBus:
        {
            state = HandleRequestBusState(event);
            break;
        }
    default:
        break;
    }
    return;
}

State StateMachine::HandleDominantState(Event event)
{
    State state = State::Dominant;
    switch (event)
    {
        case BusRequested:
        {
            DominantStateExit();
            state = State::Slave;
            SlaveStateEntry();
            break;
        }
        case ButtonPressed:
        {
            state = State::Dominant;
            break;
        }
        default:
            break;
    }
    return state;
}

State StateMachine::HandleSlaveState(Event event)
{
    Serial.print("in state");
    State state = State::Slave;
    switch (event)
    {
        case ButtonPressed:
        {
            SlaveStateExit();
            state = State::RequestingBus;
            RequestingBusStateEntry();
            break;
        }

        default:
            break;
    }
    return state;
}

State StateMachine::HandleRequestBusState(Event event)
{
    Serial.print("Request bus");
    State state = State::RequestingBus;
    switch (event)
    {
        case BusControlTaken:
        {
            RequestBusStateExit();
            state = State::Dominant;
            DominantStateEntry();
            break;
        }
        default:
            break;
        
    }
    return state;
}

// void StateMachine::Initialization(DeviceType type)
// {
//     switch (type)
//     {
//         case Gas:
//         {
//             break;
//         }
//         case DHT:
//         {
//             break;
//         }
//     default:
//         break;
//     }
// }

void StateMachine::DominantStateEntry()
{
    //Wire.begin();
}

void StateMachine::DominantStateExit()
{
    //Wire.end();
}

void StateMachine::SlaveStateEntry()
{
    //Wire.begin(9);
}

void StateMachine::SlaveStateExit()
{

}

void StateMachine::RequestingBusStateEntry()
{

}

void StateMachine::RequestBusStateExit()
{
    Wire.end();
}
            