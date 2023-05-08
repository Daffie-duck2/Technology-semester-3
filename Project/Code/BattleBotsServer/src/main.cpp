#include "SocketServer.h"
#include "GameManager.h"
#include <stdio.h>

#define PORT 8080

int main (void)
{
    GameManager gameManager;
    printf("Start server.....\n");
    
    if (gameManager.SetupGameManager() == 0)
    {
        printf("Server setup complete!\n");
    }
    else 
    {
        printf("server setup gone wrong\n");
    }

    while (1)
    {
        gameManager.HandleGameManager();
    }
}