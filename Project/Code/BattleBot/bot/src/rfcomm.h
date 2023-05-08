#ifndef RFCOMM_H
#define RFCOMM_H

#include <WiFiUdp.h>

class RFComm {
public:
    RFComm();
    virtual ~RFComm();

    bool Init();
    void Update();

    int ReadXAxis();
    int ReadYAxis();
    bool ReadButton();

private:
    int xaxis;
    int yaxis;
    bool button;

    WiFiUDP Udp;
};

#endif