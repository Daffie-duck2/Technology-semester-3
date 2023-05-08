#include <Arduino.h>
#include <SFE_MicroOLED.h>
#include <Wire.h>
//#include "MQ2.h"
#include "StateMachine.h"

#define SensorPin A0

#define Display 42
#define GasDevice 13

#define ButtonPin 8
#define Ledpin 10

#define Reset_Pin 4
#define Jumper 5

StateMachine state;
MicroOLED oled(Reset_Pin,Jumper);
//MQ2 mq2(GasDevice);

// int buttonState = 0;
// unsigned long previousMillis = 0;
// const long interval = 1000;
// char message[80];
// char slaveMessage[80];

// char tijdelijk[] = "POLL";

void WriteToRegister(int temp)
{
  oled.setCursor(0,0);
  oled.write(temp);
  oled.display();
}

// void receiveEvent(int howMany) 
// {
//   int i = 0;
//   while (1 < Wire.available()) { // loop through all but the last
//     char c = Wire.read(); // receive byte as a character
//     message[i++] = c;
//   }   
//   Serial.print("Receive!");
//   message[i] = '\0';    
// }



// void SendMessage(char data[])
// { 
//   Wire.beginTransmission(9);
//   for(int i = 0; i <= strlen(data); i++)
//   {
//     Wire.write((uint8_t)data[i]);
//   }
//   Wire.endTransmission();
//   // if(!slave){
//   //   Wire.requestFrom(9,3);
//   //   receiveEvent(0);
//   // }
// }

// void requestHandler()
// {
//   SendMessage(slaveMessage);
//   slaveMessage[0] = '\0';
// }

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   pinMode(ButtonPin, INPUT_PULLUP);
//   pinMode(Ledpin, OUTPUT);
//   Wire.begin(0x2);
//   Wire.onReceive(receiveEvent);
//   //Wire.onRequest(requestHandler);
//   display.begin();
//   display.clear(ALL);
//   display.display();
//   message[0] = '\0';
//   //mq2.begin();
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   while(state.state == State::Slave)
//   {
//     int gas = analogRead(GasDevice);
//     //Serial.print("hoi: ");
//     // Serial.print(message);
//     // Serial.println("Doei");
//     if(!strcmp(message, "POLL"))
//     {
//       Serial.print("Poll");
//       message[0] = '\0';
    
//       //Serial.println("Ik ben de slave");
//       digitalWrite(Ledpin, HIGH);
//       if(digitalRead(ButtonPin) == HIGH)
//       {
//         Serial.println("Button ingedrukt");
//         digitalWrite(Ledpin, LOW);
//         SendMessage("RBR");
        
//         state.EventHandler(ButtonPressed);
//       }
//       else
//       {
//         SendMessage("NBC");
//       }
//     }
//     delay(100);
//   }
// //Serial.println("ik ben hier");
//   if(strcmp(message, "BRL") == 0)
//     {
//       message[0] = '\0';
//       state.EventHandler(BusControlTaken);
//     }
//   while(state.state == State::Dominant)
//   {
//     int gas = analogRead(GasDevice);
//     unsigned long currentMillis = millis();
//     if(currentMillis - previousMillis >= interval)
//     {
//       previousMillis = currentMillis;

//       WriteToRegister(gas);
//       display.clear(ALL);
//       SendMessage("POLL");
//       Serial.print(message);
//       if(!strcmp(message, "RBR"))
//       {
//         message[0] = '\0';
//         SendMessage("BRL");
//         if(!strcmp(message, "BCT"))
//         {
//           message[0] = '\0';
//           state.EventHandler(BusRequested);
//           SendMessage("OK");
//         }
//         else
//         {
//           break;
//         }
//       }
//     }
//   }
// }
void setup()
{
  delay(100);
  Wire.begin(0x4);
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
}

void loop()
{
  // if(state.state == State::Dominant)
  // {
  //   oled.print("Hello, world");
  //   oled.display();
  // }
  // else{
  //   oled.print("no world");
  //   oled.display();
  // }
}