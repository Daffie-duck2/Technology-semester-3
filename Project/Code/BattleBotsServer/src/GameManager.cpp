#include "GameManager.h"

#include <iostream>
#include <sstream>

GameManager::GameManager()
{
    currentGameId = 0;
    pingDelayTime = 5;

    lastPingSendTime = clock();
    lastPingCheckTime = clock();
    currentTime = clock();
}

GameManager::~GameManager()
{
    delete server;
    server = nullptr;

    UIThread.join();
    SocketThread.join();
}

int GameManager::SetupGameManager()
{
    try
    {
        server = new SocketServer(8080, this);
        userInterface = new UserInterface(this);
    }
    catch(const char* msg)
    {
        return -1;
    }
    
    if (server->SetupServer() != 0)
    {
        return -1;
    }

    UIThread = std::thread(&UserInterface::HandleUserInterface, userInterface);
    SocketThread = std::thread(&SocketServer::HandleSocketActivity, server);
    return 0;
}

void GameManager::HandleGameManager()
{
    HandlePing();
    HandleGames();
    HandleMessages();
}

void GameManager::NewMessageReceived(int id, std::string &message)
{
    std::pair<int, std::string> *buffer = new std::pair<int, std::string>(id, message);
    messageBuffer.push_back(buffer);
    std::string uiMessage = "new message from client: ";
    uiMessage += std::to_string(id);
    uiMessage += message;
    userInterface->Print(uiMessage);

    
;}

void GameManager::NewClient(int id)
{
    Robot *r = new Robot(id);
    Robots.push_back(r);
    std::string uiMessage = "new client connected: ";
    uiMessage += std::to_string(id);
    userInterface->Print(uiMessage);
}

void GameManager::ClientDisconnected(int id)
{
    RemoveRobot(id);

    std::string uiMessage = "client disconnected: ";
    uiMessage += std::to_string(id);
    userInterface->Print(uiMessage);
}

void GameManager::NewUIMessageReceived(std::string &message)
{
    std::vector<std::string> stringlist;
    std::istringstream stream(message);

    std::string temp;
    while (getline(stream, temp, ':'))
    {
        stringlist.push_back(temp);
    }

    temp = stringlist[0];

    if (temp == "gameStart")
    {
        StartGame(stringlist[1]);
    }
    if (temp == "robots")
    {
        std::string message("print robot list");
        userInterface->Print(message);
        PrintRobotList();
    }
    if (temp == "gameClose")
    {
        std::string message("close game");
        userInterface->Print(message);
        CloseGame(std::stoi(stringlist[1]));
    }
    if (temp == "games")
    {
        std::string message("print game list");
        userInterface->Print(message);
        PrintGameList();
    }
}

void GameManager::SendGameMessage(int id, std::string &message)
{
    server->SendMessage(id, message.c_str());
}

void GameManager::UiPrint(std::string &message)
{
    userInterface->Print(message);
}

void GameManager::HandlePing()
{
    currentTime = clock();

    if (currentTime >= (lastPingSendTime + (pingDelayTime*CLOCKS_PER_SEC)))
    {
        lastPingSendTime = currentTime;
        server->PingSockets();
    }

//    if (currentTime >= (lastPingCheckTime + 3*(pingDelayTime*CLOCKS_PER_SEC)))
//    {
//        lastPingCheckTime = currentTime;
//std::string message("Check pings");
//userInterface->Print(message);
//
//        std::list<int> connectedRobots;
//
//        for (auto message : messageBuffer)
//        {
//            if (message->second == "#PACK%")
//            {
//
//                connectedRobots.push_back(message->first);
//            }
//        }
//
//        for (Robot *robot : Robots)
//        {
//            robot->SetConnected(false);
//            for (int r : connectedRobots)
//            {
//
//                if (r == robot->GetId())
//                {
//                    robot->SetConnected(true);
//                }
//            }
//        }
//
//        std::list<int> disconnectIds;
//        for (Robot *robot : Robots)
//        {
//            if (robot->GetConnected() == false)
//            {
//                disconnectIds.push_back(robot->GetId());
//std::string message("Robot not connected");
//userInterface->Print(message);
//            }
//        }
//
//        for (auto id : disconnectIds)
//        {
//            RemoveRobot(id);
//        }
//        
//    }
}

void GameManager::HandleGames()
{
    for (auto g : Games)
    {
        g->HandleGame();
        if (g->GetCurrentlyPlaying() == false)
        {
            std::list<int> robotIds = g->GetRobotIds();
            for (auto id : robotIds)
            {
                for (auto r : Robots)
                {
                    if (id == r->GetId())
                    {
                        r->SetInGame(false);
                    }
                }
            } 
        }
    }
}

void GameManager::HandleMessages()
{
    if (messageBuffer.size() <= 0)
    {
        return;
    }

    std::pair<int, std::string> *message = messageBuffer.front();
    messageBuffer.pop_front();

    std::string messageString = message->second;
    int messageId = message->first;
    
    if (messageString.front() == '#' && messageString.back() == '%')
    {
        server->SendMessage(messageId, "#ACK%");
    }
    else
    {
        server->SendMessage(messageId, "#NACK%");
        return;
    }

    std::vector<std::string> stringlist;
    std::istringstream stream(messageString);

    std::string temp;
    while (getline(stream, temp, ':'))
    {
        stringlist.push_back(temp);
    }
    temp = stringlist[0];

    for (auto g : Games)
    {
        std::list<int> robotIds = g->GetRobotIds();
        for (auto id : robotIds)
        {
            if (id == messageId)
            {
                g->NewGameMessage(messageString, messageId);
            }
        }
    }
}

void GameManager::PrintRobotList()
{
    for (Robot *r : Robots)
    {
        std::string robotString = "Robot id: " + std::to_string(r->GetId()) + " InGame: " + std::to_string(r->GetInGame());
        userInterface->Print(robotString);
    }
}

void GameManager::PrintGameList()
{
    std::string gameString;
    for (auto g : Games)
    {
        gameString = "";
        gameString += "Game id: " + std::to_string(g->GetId()) + " Playing: " + std::to_string(g->GetCurrentlyPlaying());
        if (g->GetCurrentlyPlaying() == false)
        {
            gameString += " Winner: " + std::to_string(g->GetWinnerId());
        }
        userInterface->Print(gameString);
    }
}

void GameManager::CloseGame(int id)
{
    for (auto g : Games)
    {
        if (id == g->GetId())
        {
            g->CloseGame();
        }
    }
}

void GameManager::RemoveRobot(int id)
{
    Robot* temp = NULL;
    for (Robot *r : Robots)
    {
        if (r->GetId() == id)
        {
            temp = r;
        }
    }
    if (temp != NULL)
    {
        server->DisconnectClient(temp->GetId());
        Robots.remove(temp);
        delete temp;
    }
}

void GameManager::StartGame(std::string &RobotIds)
{
    std::vector<std::string> stringlist;
    std::istringstream stream(RobotIds);

    std::string temp;
    while (getline(stream, temp, ','))
    {
        stringlist.push_back(temp);
    }

    if (stringlist.size() <= 1)
    {
        std::string message("error, not enough robots to start the game");
        userInterface->Print(message);
        return;
    }

    std::list<int> robotIdList;
    for (auto s : stringlist)
    {
        try
        {
            int robotId = std::stoi(s);
            robotIdList.push_back(robotId);
            for (auto r : Robots)
            {
                if (robotId == r->GetId())
                {
                    if (r->GetInGame() == true)
                    {
                        std::string error("ERROR: robot is already in game, id: ");
                        error += std::to_string(robotId);
                        userInterface->Print(error);
                        return;
                    }
                    r->SetInGame(true);
                }
            }
        }
        catch(const std::exception& e)
        {
            std::string error("ERROR: could not convert robotId to int");
            userInterface->Print(error);
            return;
        }
    }

    Game *newGame = new Game(this,robotIdList, currentGameId);
    currentGameId++;
    Games.push_back(newGame);
}
