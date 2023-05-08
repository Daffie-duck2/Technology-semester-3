#include <Arduino.h>
#include <DHT.h>
#include <SFE_MicroOLED.h>
#include "Wire.h"
#include "StateMachine.h"

#define SensorPin A0

#define Display 42

#define DHTHumidity 2

#define ButtonPin 3

#define Reset_Pin 4
#define Jumper 5

#define DHTType DHT11

DHT dht(DHTHumidity,DHTType);
StateMachine state;
MicroOLED oled(Reset_Pin,Jumper);

// int buttonState = 0;
// unsigned long previousMillis = 0;
// const long interval = 1000;
// char message[80];
// char slaveMessage[80];

void WriteToRegister(int temp)
{
  oled.setCursor(0,0);
  oled.write(temp);
  oled.display();
  //previousMillis = 0;
}


// void receiveEvent(int howMany) {
  
//   int i = 0;
//   while (1 < Wire.available()) { // loop through all but the last
//     char c = Wire.read(); // receive byte as a character
//     message[i++] = c;
//   }  
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
// //   if(!slave){
// //     Wire.requestFrom(9,3);
// //     receiveEvent(0);
// //   }
//  }

// void requestHandler()
// {
//   SendMessage(slaveMessage);
//   slaveMessage[0] = '\0';
// }

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   pinMode(ButtonPin, INPUT);
//   Wire.begin(0x1);
//   Wire.onReceive(receiveEvent);
//   //Wire.onRequest(requestHandler);
//   display.begin();
//   display.clear(ALL);
//   display.display();
//   message[0] = '\0';
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   dht.read(SensorPin);
//   buttonState = digitalRead(ButtonPin);
//   int temp = dht.readTemperature();
//   while(state.state == State::Slave)
//   {
//     // if(buttonState == HIGH)
//     // {
//     //   buttonState = 0;
//     //   SendMessage("RBR");
//     //   state.EventHandler(ButtonPressed);
//     // }
//     if(!strcmp(message, "BRL"))
//     {
//       message[0] = '\0';
//       state.EventHandler(BusControlTaken);
//     }
//   }
  
//   while(state.state == State::Dominant)
//   {
//    //Serial.println("Ik ben de dom");
//     unsigned long currentMillis = millis();
//     if(currentMillis - previousMillis >= interval)
//     {
//       previousMillis = currentMillis;
//       WriteToRegister(temp);
//       display.clear(ALL);
//       SendMessage("POLL");
//       if(!strcmp(message, "RBR"))
//       {
//         Serial.print("Hoi");
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
  Wire.begin(0x6);
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
}

void loop()
{
  oled.setCursor(0,0);
  oled.print("Hello, world");
  oled.display();
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

