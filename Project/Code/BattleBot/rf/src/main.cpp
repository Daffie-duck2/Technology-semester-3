#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>

#include <stdint.h>
#include <string.h>

char* ssid = "Lowie iPhone";
char* password = "Battlebot123";

IPAddress serverIp(172, 20, 10, 9);
const uint16_t port = 2345;

WiFiUDP client;

void setup()
{
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED);
    client.begin(port);
}

void loop()
{
    uint16_t x = analogRead(32);
    uint16_t y = analogRead(33);

    uint8_t buffer[] = {
        x,
        x >> 8,
        y,
        y >> 8
    };

    /*char buffer[15];
    sprintf(&buffer[0], "%04d:%04d\n", x, y);*/
    
/*Serial.println(buffer);*/
/*Serial.print(x);
Serial.print(":");
Serial.println(y);*/

    client.beginPacket(serverIp, port);
    client.write(buffer, sizeof(buffer));
    client.endPacket();

    delay(60);
}