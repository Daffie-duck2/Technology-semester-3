#ifndef ROBOT
#define ROBOT

#include <string>

class Robot
{
private:
    int id;
    bool connected;
    bool inGame;

public:
    Robot(int id);
    virtual ~Robot();

    bool operator==(const Robot& other) const;

    int GetId();
    bool GetConnected();
    void SetConnected(bool state);

    bool GetInGame();
    void SetInGame(bool state);
    std::string ToString();

};




#endif