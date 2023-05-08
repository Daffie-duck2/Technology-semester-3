#include "Game.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <limits.h>

Game::Game(GameCallback* gameCallback, std::list<int> robotIds, int id)
    : gameCallback(gameCallback)
    , id(id)
    , winnerId(-1)
    , currentGameState(GameState::WaitingToStart)
    , gameTime(30)
    , gameStartDelay(3)
{
    std::string gameSetupString = "#Game:";
    gameSetupString += std::to_string(id);
    gameSetupString += ":";
    for (auto r : robotIds)
    {
        std::pair<int, int> *healthPair = new std::pair<int, int>(r, 0);
        std::pair<int, int> *hitPair = new std::pair<int, int>(r, 0);
        robotHeatlhList.push_back(healthPair);
        robotHitList.push_back(hitPair);
        gameSetupString += std::to_string(r);
        gameSetupString +=",";
    }
    gameSetupString.pop_back();
    gameSetupString += "%";

    for (auto r : robotIds)
    {
        gameCallback->SendGameMessage(r, gameSetupString);
    }

    currentTime = clock();
    gameCreateTime = clock();
    gameStartTime = clock();
}

Game::~Game()
{
    for (auto r : robotHeatlhList)
    {
        delete r;
    }
    robotHeatlhList.clear();

    for (auto r : robotHitList)
    {
        delete r;
    }
    robotHitList.clear();
}

void Game::HandleGame()
{
    currentTime = clock();

    switch (currentGameState)
    {
    case GameState::WaitingToStart:
    {
        if (currentTime - gameCreateTime > gameStartDelay*CLOCKS_PER_SEC)
        {
            for (auto r : robotHeatlhList)
            {
                std::string message("#GameStart%");
                gameCallback->SendGameMessage(r->first, message);
            }
            std::string message("Game started! id: ");
            message += std::to_string(id);
            gameCallback->UiPrint(message);
            gameStartTime = currentTime;
            currentGameState = GameState::Playing;
        }
    }
        break;

    case GameState::Playing:
    {
        bool gameStop = false;
        if (currentTime - gameStartTime > gameTime*CLOCKS_PER_SEC)
        {
            gameStop = true;
        }
        
        for (auto r : robotHeatlhList)
        {
            if (r->second >= 5)
            {
                gameStop = true;
            }
        }

        for (auto r : robotHitList)
        {
            if (r->second >= 5)
            {
                gameStop = true;
            }
        }

        if (gameStop == true)
        {
            CloseGame();
        }
    }
        break;

    case GameState::Done:
    {
        //Wait for game manager to get scores and delete game
    }
        break;
    
    default:
        break;
    }
}

void Game::NewGameMessage(std::string &message, int id)
{
    std::vector<std::string> stringlist;
    std::istringstream stream(message);

    std::string temp;
    while (getline(stream, temp, ':'))
    {
        stringlist.push_back(temp);
    }

    if (stringlist.front() == "Hit")
    {
        try
        {
            int shootingRobotId = std::stoi(stringlist.back());
            std::string message;
            message += "Game: " + std::to_string(this->id) + " Robot: " + std::to_string(id) + " Hit by: " + std::to_string(shootingRobotId);
            gameCallback->UiPrint(message);
            for (auto r : robotHeatlhList)
            {
                if (r->first == id)
                {
                    r->second++;
                }
            }
            for (auto r : robotHitList)
            {
                if (r->first == shootingRobotId)
                {
                    r->second++;
                }
            }
        }
        catch(const std::exception& e)
        {
            std::string error("ERROR: could not convert robotId to int");
        }
    }
}

void Game::CloseGame()
{
    for (auto r : robotHeatlhList)
    {
        std::string message("#GameStop%");
        gameCallback->SendGameMessage(r->first, message);
    }
    std::string message("Game stopped! id: ");
    message += std::to_string(id);
    gameCallback->UiPrint(message);
    winnerId = CalculateWinner();
    currentGameState = GameState::Done;
}

bool Game::GetCurrentlyPlaying()
{
    if (currentGameState == GameState::Done)
    {
        return false;
    }
    return true;
}

int Game::GetId()
{
    return id;
}

int Game::GetWinnerId()
{
    return winnerId;
}

std::list<int> Game::GetRobotIds()
{
    std::list<int> list;
    for (auto r : robotHeatlhList)
    {
        list.push_back(r->first);
    }
    return list;
}

int Game::CalculateWinner()
{
    int lowestHealth = INT_MAX;
    int winner = -1;
    for (auto pair : robotHeatlhList)
    {
        if (pair->second < lowestHealth)
        {
            lowestHealth = pair->second;
            winner = pair->first;
        }
    }
    return winner;
}
