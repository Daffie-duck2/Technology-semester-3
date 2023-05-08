#include "StateMachine.h"
#include <Arduino.h>

/*
ReceivedGameID,
  GameStarted,
  GameStopped,
  None
*/

static State currentState = WaitingForGame;

State getCurrentState()
{
    return currentState;
}

void setCurrentState(State s)
{
    currentState = s;
}

State HandleWaitingForGameState(Event event)
{
    State state = WaitingForGame;
    switch(event) 
    {
        case ReceivedGameID:
        Serial.println("RECEIVED ID");
            state = WaitingForGameStart;
            break;
        default:
            break;
    }
    return state;
}

State HandleWaitingForGameStartState(Event event)
{
    State state = WaitingForGameStart;
    switch(event) 
    {
        case GameStarted:
            Serial.println("GAME START");
            state = Gaming;
            break;
        default:
            break;
    }
    return state;
}

State HandleGamingState(Event event)
{
    State state = Gaming;
    switch(event) 
    {
        case GameStopped:
            state = WaitingForGame;
            break;
        default:
            break;
    }
    return state;
}

void EventHandler(Event event)
{
    switch(currentState)
    {
        case WaitingForGame:
            Serial.println("IN STATE WATINGFORGAME");
            setCurrentState(HandleWaitingForGameState(event));
            break;
        case WaitingForGameStart:
            Serial.println("IN STATE WATINGFORGAMESTART");
            setCurrentState(HandleWaitingForGameStartState(event));
            break;
        case Gaming:
            Serial.println("IN STATE GAMING");
            setCurrentState(HandleGamingState(event));
            break;
        default:
            break;
    }
}