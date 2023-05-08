#ifndef GAMECALLBACK
#define GAMECALLBACK

#include <string>

class GameCallback
{
public:
    ~GameCallback() {}
    virtual void SendGameMessage(int id, std::string &message) = 0;
    virtual void UiPrint(std::string &message) = 0;
};

#endif