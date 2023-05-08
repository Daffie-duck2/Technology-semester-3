#include "UserInterface.h"

#include <iostream>

UserInterface::UserInterface(UserInterfaceCallback* userInterfaceCallback)
    : userInterfaceCallback(userInterfaceCallback)
    , quit(false)
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::HandleUserInterface()
{
    while (!quit)
    {
        std::string input = "u";
        PrintMenu();
        std::getline(std::cin, input);

        if (input == "quit")
        {
            quit = true;
        }
        else
        {
            userInterfaceCallback->NewUIMessageReceived(input);
        }
    }
    return;
}

void UserInterface::Print(std::string &message)
{
    std::cout << message << std::endl;
}

void UserInterface::PrintMenu()
{
    std::cout << std::endl;
    std::cout << "Menu:" << std::endl;
    std::cout << "gameStart:robotids to start game" << std::endl;
    std::cout << "games to get a list of all games" << std::endl;
    std::cout << "robots to show connected robots" << std::endl;
    std::cout << "gameClose:gameid to close game" << std::endl;
}