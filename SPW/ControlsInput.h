#pragma once

#include "Settings.h"
#include "InputGroup.h"

class ControlsInput : public InputGroup
{
public:
    ControlsInput();

    void OnPreEventProcess() override;
    void OnEventProcess(SDL_Event evt) override;
    void Reset() override;

    float hAxis = 0.f;
    bool jumpDown = false;
    bool jumpPressed = false;
    bool goDownDown = false;
    bool pausePressed = false;
    bool specialPressed = false;
};
