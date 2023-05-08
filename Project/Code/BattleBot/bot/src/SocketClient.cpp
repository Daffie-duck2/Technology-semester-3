#include "WiFiType.h"
#include "SocketClient.h"

#include <stdexcept>

SocketCommunication::SocketCommunication(IPAddress serverIp, uint16_t port, char* ssid, char* password)
  : serverIp(serverIp)
  , port(port)
  , ssid(ssid)
  , password(password)
{
  if (ssid == nullptr || password == nullptr)
  {
    throw std::invalid_argument("invalid ssid or password given");
  }

  client = new WiFiClient();
}

SocketCommunication::~SocketCommunication()
{
  client->stop();
  WiFi.disconnect();
  delete client;
  client = nullptr;
}
  
int SocketCommunication::init()
{
  wifiConnect();

  int wifiTries = 0;
  while (!wifiConnected())
  {
    if (wifiTries >= 10)
    {
      return -1;
    }

    delay(500);
    wifiTries++;
  }

  int serverTries = 0;
  while (client->connect(serverIp, port) == false)
  {
    if (serverTries >= 10)
    {
      return -2;
    }

    delay(500);
    serverTries++;
  }
  return 0;
}

bool SocketCommunication::connected()
{
  return client->connected();
}
  
int SocketCommunication::ReadIncomingMessage(std::string* message)
{
  int availableBytes = client->available();
  if (availableBytes == 0)
  {
    return -1;
  }

  for (int i = 0; i < availableBytes; i++)
  {
    *message += client->read();
  }

  return 0;
}

int SocketCommunication::SendMessageToServer(char* message)
{
  client->print(message);
  return 0;
}

void SocketCommunication::wifiConnect()
{
  WiFi.begin(ssid, password);
}

bool SocketCommunication::wifiConnected()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    return false;
  }
  return true;
}