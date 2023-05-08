#ifndef SERVERCALLBACK
#define SERVERCALLBACK

#include <string>

class ServerCallback
{
public:
    ~ServerCallback() {}
    virtual void NewMessageReceived(int id, std::string &message) = 0;
    virtual void NewClient(int id) = 0;
    virtual void ClientDisconnected(int id) = 0;
};

#endif