#include <Arduino.h>
#include <analogWrite.h>
#include "rfcomm.h"
#include "SocketClient.h"
#include "IRReceiver.h"
#include "StateMachine.h"

IPAddress serverIp(172, 20, 10, 7);
const uint16_t port = 8080;

// char* ssid = "Lowie iPhone";
// char* password = "Battlebot123";


// 32 33 25 26
#define LeftBack 25
#define LeftForward 26
#define RightBack 32
#define RightForward 33

#define PWMRightMotor 5
#define PWMLeftMotor 18

//x value 1900 2200
// y value 1900 2200

#define LowXDeadzone 1750
#define HighXDeadzone 2000
#define LowYDeadzone 1750
#define HighYDeadzone 2000

#define MaxValue 4095

#define PWMMax 700
#define PWMLow 100

#define XValue 2
#define YValue 4

int xPosition = 0;
int yPosition = 0;
int mapX = 0;
int mapY = 0;

void SwitchLeftMotorOff();
void SwitchRightMotorOff();
void ReadJoystickValue(int xValue, int yValue);

SocketCommunication socketCommunication(serverIp, port, "Lowie iPhone", "Battlebot123");
RFComm rfcommunication;

uint32_t ID;

void ReadJoystickValue(int xValue, int yValue)
{
  yPosition = xValue;
  xPosition = yValue;

  if (yPosition > HighYDeadzone) // backwards
  {
    if (xPosition < LowXDeadzone) // turn backwards left -> right motor high
    {
      // analogWrite(PWMLeftMotor, (int)map(yPosition, 2110, 4095, 100, 700));
      // analogWrite(PWMRightMotor, (int)map(xPosition, 0, 1180, 600, 100));
      analogWrite(PWMLeftMotor, (int)map(xPosition, 0, LowXDeadzone, PWMMax, PWMLow));
      analogWrite(PWMRightMotor, (int)map(yPosition, HighYDeadzone, MaxValue, PWMLow, PWMMax));
      digitalWrite(LeftForward, LOW);
      digitalWrite(LeftBack, HIGH);
      digitalWrite(RightForward, LOW);
      digitalWrite(RightBack, HIGH);
    }
    else if (xPosition > HighXDeadzone) // turn backwards right -> left motor high
    {
      // analogWrite(PWMLeftMotor, (int)map(yPosition, 1710, 2110,0,100));
      // analogWrite(PWMRightMotor, (int)map(xPosition, 1580, 4095, 100, 700));
      analogWrite(PWMLeftMotor, (int)map(yPosition, HighYDeadzone, MaxValue, PWMLow, PWMMax));
      analogWrite(PWMRightMotor, (int)map(xPosition, HighXDeadzone, MaxValue, PWMLow, PWMMax));
      digitalWrite(LeftForward, LOW);
      digitalWrite(LeftBack, HIGH);
      digitalWrite(RightForward, LOW);
      digitalWrite(RightBack, HIGH);
    }
    else // x is in the dead zone
    {
      // analogWrite(PWMLeftMotor, (int)map(yPosition, 2110, 4095, 100, 700));
      // analogWrite(PWMRightMotor, (int)map(xPosition, 0, 1180, 600, 100));
      analogWrite(PWMLeftMotor, (int)map(yPosition, HighYDeadzone, MaxValue, PWMLow, PWMMax));
      analogWrite(PWMRightMotor, (int)map(yPosition, HighYDeadzone, MaxValue, PWMLow, PWMMax));
      digitalWrite(LeftForward, LOW);
      digitalWrite(LeftBack, HIGH);
      digitalWrite(RightForward, LOW);
      digitalWrite(RightBack, HIGH);
    }
  }
  else if (yPosition < LowYDeadzone) // forward
  {
    if (xPosition < LowXDeadzone) // turn forwards left -> right motor high
    {
      // analogWrite(PWMLeftMotor, (int)map(yPosition, 2110, 4095, 100, 700));
      // analogWrite(PWMRightMotor, (int)map(xPosition, 0, 1180, 600, 100));
      analogWrite(PWMLeftMotor, (int)map(xPosition, 0, LowXDeadzone, PWMLow, PWMMax));
      analogWrite(PWMRightMotor, (int)map(yPosition, 0, LowYDeadzone, PWMMax, PWMLow));
      digitalWrite(LeftForward, HIGH);
      digitalWrite(LeftBack, LOW);
      digitalWrite(RightForward, HIGH);
      digitalWrite(RightBack, LOW);
    }
    else if (xPosition > HighXDeadzone) // turn forwards right -> left motor high
    {
      // analogWrite(PWMLeftMotor, (int)map(yPosition, 1710, 2110,0,100));
      // analogWrite(PWMRightMotor, (int)map(xPosition, 1580, 4095, 100, 700));
      analogWrite(PWMLeftMotor, (int)map(xPosition, HighXDeadzone, MaxValue, PWMLow, PWMMax));
      analogWrite(PWMRightMotor, (int)map(yPosition, 0, LowYDeadzone, PWMLow, PWMMax));
      digitalWrite(LeftForward, HIGH);
      digitalWrite(LeftBack, LOW);
      digitalWrite(RightForward, HIGH);
      digitalWrite(RightBack, LOW);
    }
    else // x is in the dead zone
    {
      analogWrite(PWMLeftMotor, (int)map(yPosition, 0, LowYDeadzone, PWMMax, PWMLow));
      analogWrite(PWMRightMotor, (int)map(yPosition, 0, LowYDeadzone, PWMMax, PWMLow));
      digitalWrite(LeftForward, HIGH);
      digitalWrite(LeftBack, LOW);
      digitalWrite(RightForward, HIGH);
      digitalWrite(RightBack, LOW);
    }
  }
  else // y in deadzone
  {
    if (xPosition < LowXDeadzone) // turn left -> right motor high
    {
      // analogWrite(PWMLeftMotor, (int)map(yPosition, 2110, 4095, 100, 700));
      // analogWrite(PWMRightMotor, (int)map(xPosition, 0, 1180, 600, 100));
      analogWrite(PWMRightMotor, (int)map(yPosition, LowYDeadzone, HighYDeadzone, 100, 0));
      analogWrite(PWMLeftMotor, (int)map(xPosition, 0, LowXDeadzone, PWMMax, PWMLow));
      digitalWrite(LeftForward, HIGH);
      digitalWrite(LeftBack, LOW);
      digitalWrite(RightForward, HIGH);
      digitalWrite(RightBack, LOW);
    }
    else if (xPosition > HighXDeadzone) // turn right -> left motor high
    {
      analogWrite(PWMLeftMotor, (int)map(yPosition, LowYDeadzone, HighYDeadzone,0,100));
      analogWrite(PWMRightMotor, (int)map(xPosition, HighXDeadzone, MaxValue, PWMLow, PWMMax));
      // analogWrite(PWMLeftMotor, (int)map(xPosition, 1580, 4095, 100, 700));
      // analogWrite(PWMRightMotor, (int)map(yPosition, 1710, 2110, 0, 100));
      digitalWrite(LeftForward, HIGH);
      digitalWrite(LeftBack, LOW);
      digitalWrite(RightForward, HIGH);
      digitalWrite(RightBack, LOW);
    }
    else // x is in the dead zone
    {
      SwitchLeftMotorOff();
      SwitchRightMotorOff();
    }
  }
}

void SwitchLeftMotorOff()
{
  digitalWrite(LeftForward, LOW);
  digitalWrite(LeftBack, LOW);
}

void SwitchRightMotorOff()
{
  digitalWrite(RightForward, LOW);
  digitalWrite(RightBack, LOW);
}

//States Events

void ServerCommunication()
{
  std::string messageBuffer;
  if (socketCommunication.ReadIncomingMessage(&messageBuffer) != -1)
  {
    Serial.println(messageBuffer.c_str());
    Serial.println("message received");
    if(strcmp(messageBuffer.c_str(), "#GameStart%") == 0){
      EventHandler(GameStarted);
    }
    else if(strcmp(messageBuffer.c_str(), "#GameStop%") == 0)
    {
      EventHandler(GameStopped);
    }
    else if(strncmp(messageBuffer.c_str(), "#Game", 5) == 0)
    {
      EventHandler(ReceivedGameID);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LeftBack, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(RightBack, OUTPUT);
  pinMode(RightForward, OUTPUT);

  pinMode(PWMRightMotor, OUTPUT);
  pinMode(PWMLeftMotor, OUTPUT);

  pinMode(XValue, INPUT);
  pinMode(YValue, INPUT);

  while (int result = socketCommunication.init() < 0)
  {
    Serial.println(result);
    Serial.println("Connection to server failed. trying again.");
  }
  Serial.println("Connection to server succes!");

  setCurrentState(WaitingForGame);

  rfcommunication.Init();

  IrReceiverSetup();
}

void loop()
{
  while(getCurrentState() == WaitingForGame)
  {
    ServerCommunication();
    rfcommunication.Update();
    ReadJoystickValue(rfcommunication.ReadXAxis(), rfcommunication.ReadYAxis());
  }

  while(getCurrentState() == WaitingForGameStart)
  {
    ServerCommunication();
  }

  while(getCurrentState() == Gaming)
  {
    ServerCommunication();
    int result = IrReceiverLoop(&ID);
    if(result == 1){
      char str[25];
      sprintf(str, "#Hit:%lu%%", ID);
      socketCommunication.SendMessageToServer(str);
    }
    rfcommunication.Update();
    ReadJoystickValue(rfcommunication.ReadXAxis(), rfcommunication.ReadYAxis());
  }
}

    // Serial.println(ID, HEX);
    // if(ID == 0xF076C13B){
    //   SwitchLeftMotorOff();
    //   Serial.println("LEFT MOTOR OFF");
    // }
    // else if(ID == 0xE5CFBD7F){
    //   SwitchRightMotorOff();
    //   Serial.println("RIGHT MOTOR OFF");
    // }