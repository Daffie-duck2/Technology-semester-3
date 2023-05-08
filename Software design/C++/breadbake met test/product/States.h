#ifndef STATES_H
#define STATES_H

enum class States
{
    STANDBY,
    IDLE,
    PROCESSING
};

enum class Substate
{
    RESTING,
    KNEADING,
    RISE,
    BAKING
};

#endif
