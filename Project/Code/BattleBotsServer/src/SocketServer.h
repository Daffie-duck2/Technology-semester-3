#ifndef socketserver
#define socketserver

#define MAXCLIENTS 30

#include <sys/time.h>
#include <netinet/in.h>
#include <string>
#include "ServerCallback.h"

class SocketServer
{
public:
    SocketServer(int port, ServerCallback* serverCallback);
    virtual ~SocketServer();

    int SetupServer();
    void HandleSocketActivity();
    int CloseSocket(int socket);
    int SendMessage(int socketId, const char* message);
    int GetClientSocket(int id);
    void DisconnectClient(int id);

    void PingSockets();

private:
    int port;
    ServerCallback* serverCallback;

    fd_set readfds;
    int master_socket;
    int opt;
    int client_socket[MAXCLIENTS];
    struct sockaddr_in address;
    int addrlen;

    int AcceptNewSocket();
};

#endif
