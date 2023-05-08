#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum {
  ReceivedGameID,
  GameStarted,
  GameStopped
} Event;

typedef enum {
  WaitingForGame,
  WaitingForGameStart,
  Gaming
} State;

State getCurrentState();
void setCurrentState(State s);

void EventHandler(Event event);

#endif