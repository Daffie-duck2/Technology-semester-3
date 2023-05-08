typedef enum 
{
    Dominant = 0,
    Slave = 1,
    RequestingBus = 2,
}State;

typedef enum
{
    ButtonPressed = 0,
    BusRequested = 1,
    BusControlTaken = 3
}Event;

// typedef enum
// {
//     Gas,
//     DHT
// }DeviceType;

class StateMachine
{
private:
    //State currentState;

    State HandleDominantState(Event event);
    State HandleSlaveState(Event event);
    State HandleRequestBusState(Event event);
    void DominantStateEntry();
    void DominantStateExit();
    void SlaveStateEntry();
    void SlaveStateExit();
    void RequestingBusStateEntry();
    void RequestBusStateExit();

    
public:
    StateMachine();
    ~StateMachine();

    void EventHandler(Event event);
    State state;
    //void Initialization(DeviceType type);
};
