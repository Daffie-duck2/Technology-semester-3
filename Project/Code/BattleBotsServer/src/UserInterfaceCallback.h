#ifndef USERINTERFACECALLBACK
#define USERINTERFACECALLBACK

#include <string>

class UserInterfaceCallback
{
public:
    ~UserInterfaceCallback() {}
    virtual void NewUIMessageReceived(std::string &message) = 0;
};

#endif
