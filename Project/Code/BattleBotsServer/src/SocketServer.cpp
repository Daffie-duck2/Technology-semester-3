#include "SocketServer.h"
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

SocketServer::SocketServer(int port, ServerCallback* serverCallback)
    : port(port)
    , serverCallback(serverCallback)
{
    opt = 1;
    for (int i = 0; i < MAXCLIENTS; i++)
    {
        client_socket[i] = -1;
    }
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        throw "Socket server constructor failed";
    }
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
    {
        throw "Socket server constructor failed";
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    addrlen = sizeof(addrlen);
}

SocketServer::~SocketServer()
{
    for (int i = 0; i < MAXCLIENTS; i++)
    {
        int sd = client_socket[i];
        close(sd);
        client_socket[i] = -1;
    }
    close(master_socket);
}

int SocketServer::SetupServer()
{
    if (bind(master_socket, (struct sockaddr*)&address, sizeof(address))<0)
    {
        return -1;
    }

    if (listen(master_socket, 3) < 0)
    {
        return -1;
    }
    return 0;
}

void SocketServer::HandleSocketActivity()
{
    while (1)
    {
    FD_ZERO(&readfds);
    FD_SET(master_socket, &readfds);
    int max_sd = master_socket;

    for (int i = 0; i < MAXCLIENTS; i++)
    {
        int sd = client_socket[i];
        if (sd > 0)
        {
            FD_SET(sd, &readfds);
        }
        if (sd > max_sd)
        {
            max_sd = sd;
        }
    }

    int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR))
    {
        return;
    }

    if (FD_ISSET(master_socket, &readfds))
    {
        int result = AcceptNewSocket();
        if (result != -1)
        {
            serverCallback->NewClient(result);
        }
        
    }

    for (int i = 0; i < MAXCLIENTS; i ++)
    {
        int sd = client_socket[i];

        if (FD_ISSET(sd, &readfds))
        {
            char buffer[1025];
            int valread;
            if ((valread = read(sd, buffer, 1024)) <= 0)
            {

                close(sd);
                client_socket[i] = -1;
                serverCallback->ClientDisconnected(i);
            }
            else
            {
                buffer[valread] = '\0';
                std::string message(buffer);
                serverCallback->NewMessageReceived(i, message);
            }
        }
    }
    }
}

int SocketServer::SendMessage(int socketId, const char* message)
{
    int socket = client_socket[socketId];
    if (socket == -1)
    {
        return -1;
    }
    send(socket, message, strlen(message), 0);
    return 0;
}

int SocketServer::GetClientSocket(int id)
{
    return client_socket[id];
}

void SocketServer::DisconnectClient(int id)
{
    int socket_Fd = client_socket[id];
    int result = close(socket_Fd);
    
    if (result == 0)
    {
        std::cout<< "socket closed";
        client_socket[id] = -1;
    }
}

void SocketServer::PingSockets()
{
    const char* message = "#Ping%";

    for (int i = 0; i < MAXCLIENTS; i++)
    {
        if (client_socket[i] != -1)
        {
            SendMessage(i, message);
        }
    }
}

int SocketServer::AcceptNewSocket()
{
    int new_socket;
    if ((new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen)) == -1)
    {
        return -1;
    }

    for (int i = 0; i < MAXCLIENTS; i++)
    {
        if (client_socket[i] == -1)
        {
            client_socket[i] = new_socket;
            return i;
        }
    }
    return -1;
}
