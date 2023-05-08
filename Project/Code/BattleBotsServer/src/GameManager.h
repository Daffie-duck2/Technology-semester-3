#ifndef GAMEMANAGER
#define GAMEMANAGER

#include "SocketServer.h"
#include "UserInterface.h"
#include "Robot.h"
#include "Game.h"
#include <list>
#include <queue>
#include <utility>
#include <thread>
#include <ctime>

class GameManager : public ServerCallback, public UserInterfaceCallback, public GameCallback
{
public:
    GameManager();
    virtual ~GameManager();

    int SetupGameManager();
    void HandleGameManager();

    void NewMessageReceived(int id, std::string &message);
    void NewClient(int id);
    void ClientDisconnected(int id);

    void NewUIMessageReceived(std::string &message);

    void SendGameMessage(int id, std::string &message);
    void UiPrint(std::string &message);

private:
    std::list<std::pair<int, std::string>*> messageBuffer;
    std::list<Robot*> Robots;
    std::list<Game*> Games;
    SocketServer* server;
    UserInterface* userInterface;
    std::thread UIThread;
    std::thread SocketThread;

    clock_t currentTime;
    clock_t lastPingSendTime;
    clock_t lastPingCheckTime;

    int currentGameId;

    int pingDelayTime;

    int StartNewGame();
    void HandlePing();
    void HandleGames();
    void HandleMessages();
    void PrintRobotList();
    void PrintGameList();
    void CloseGame(int id);
    void RemoveRobot(int id);

    void StartGame(std::string &RobotIds);
};

#endif
