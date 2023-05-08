#ifndef USERINTERFACE
#define USERINTERFACE

#include <string>
#include "UserInterfaceCallback.h"

class UserInterface
{
private:
    UserInterfaceCallback* userInterfaceCallback;
    bool quit;

    void PrintMenu();
public:
    UserInterface(UserInterfaceCallback* userInterfaceCallback);
    ~UserInterface();
    
    void HandleUserInterface();
    void Print(std::string &message);
};




#endif
