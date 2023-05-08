#include "BreadBaker.h"
#include "BrokenLog.h"
#include "MDisplay.h"
#include "MEventGenerator.h"
#include "MExtraIngredientsTray.h"
#include "MKneadMotor.h"
#include "MOven.h"
#include "MStartButtonLed.h"
#include "MTimer.h"
#include "MYeastTray.h"
#include "TimeConstants.h"

using ::testing::Return;
using ::testing::_;

class IDLE: public ::testing::Test
{
protected:
    IDLE()
    {
        baker = new BreadBaker(
                    oven, timer, motor, yeast, extras,
                    display, startButton, event, log);
    }

    virtual ~IDLE()
    {
        delete baker;
        baker = nullptr;
    }

    MDisplay display;
    MEventGenerator event;
    MExtraIngredientsTray extras;
    MKneadMotor motor;
    MOven oven;
    MStartButtonLed startButton;
    MTimer timer;
    MYeastTray yeast;

    BrokenLog log;
    BreadBaker* baker;
};

TEST_F(IDLE, menu_button_pressed_event)
{
    EXPECT_EQ(States::IDLE, baker->HandleIdleState(Events::MENU_BUTTON_PRESSED));

}

TEST_F(IDLE, menu_button_long_pressed_event)
{
    EXPECT_EQ(States::STANDBY, baker->HandleIdleState(Events::MENU_BUTTON_LONG_PRESSED));
}

TEST_F(IDLE, timer_timeout_event)
{
    EXPECT_EQ(States::STANDBY, baker->HandleIdleState(Events::TIMER_TIMEOUT));
}

TEST_F(IDLE, start_button_event)
{
    EXPECT_EQ(States::PROCESSING, baker->HandleIdleState(Events::START_BUTTON_PRESSED));
    
}