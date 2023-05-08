#include "Robot.h"

Robot::Robot(int id)
    : id(id)
    , connected(true)
    , inGame(false)
{
}

Robot::~Robot()
{
}

int Robot::GetId()
{
    return id;
}

bool Robot::operator==(const Robot& other) const
{
    if (this->id == other.id)
    {
        return true;
    }
    return false;
}

bool Robot::GetConnected()
{
    return connected;
}

void Robot::SetConnected(bool state)
{
    connected = state;
}

bool Robot::GetInGame()
{
    return inGame;
}

void Robot::SetInGame(bool state)
{
    inGame = state;
}

std::string Robot::ToString()
{
    return "Robot: " + std::to_string(id);
}
