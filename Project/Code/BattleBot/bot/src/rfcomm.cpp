#include "rfcomm.h"
#include <Arduino.h>
#include <WiFiUdp.h>

RFComm::RFComm()
{
    xaxis = 1900;
    yaxis = 1900;
}

RFComm::~RFComm()
{

}

bool RFComm::Init()
{
    Udp.begin(2345);

    return true;
}

void RFComm::Update()
{
    if (Udp.parsePacket() >= 4)
    {
        this->xaxis = Udp.read() | (Udp.read() << 8);
        this->yaxis = Udp.read() | (Udp.read() << 8);
    }
}

int RFComm::ReadXAxis()
{
    return xaxis;
}

int RFComm::ReadYAxis()
{
    return yaxis;
}

bool RFComm::ReadButton()
{
    return button;
}