#ifndef GAME
#define GAME

#include <list>
#include "GameCallback.h"

enum class GameState{WaitingToStart, Playing, Done};

class Game
{
private:
    GameCallback* gameCallback;
    int id;
    int winnerId;
    std::list<std::pair<int, int>*> robotHeatlhList;
    std::list<std::pair<int, int>*> robotHitList;

    clock_t currentTime;
    clock_t gameCreateTime;
    clock_t gameStartTime;

    GameState currentGameState;

    int gameTime;
    int gameStartDelay; 

    int CalculateWinner();

public:
    Game(GameCallback *gameCallback, std::list<int> robotIds, int id);
    virtual ~Game();

    void HandleGame();
    void NewGameMessage(std::string &message, int id);
    void CloseGame();
    bool GetCurrentlyPlaying();
    int GetId();
    int GetWinnerId();
    std::list<int> GetRobotIds();
};

#endif
