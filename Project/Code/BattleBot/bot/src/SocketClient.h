#ifndef SOCKETCLIENT
#define SOCKETCLIENT

#include <WiFi.h>
#include <SPI.h>
#include <string.h>

class SocketCommunication
{
private:
  IPAddress serverIp;
  uint16_t port;
  char* ssid;
  char* password;
  WiFiClient *client;

  void wifiConnect();
  bool wifiConnected();

public:
  SocketCommunication(IPAddress serverIp, uint16_t port, char* ssid, char* password);
  ~SocketCommunication();
  
  int init();
  bool connected();
  int ReadIncomingMessage(std::string* message);
  int SendMessageToServer(char* message);

};

#endif