#pragma once

#include "Settings.h"
#include "InputGroup.h"

class ApplicationInput : public InputGroup
{
public:
    ApplicationInput();

    void OnPreEventProcess() override;
    void OnEventProcess(SDL_Event evt) override;
    void Reset() override;

    bool quitPressed;
    bool pausePressed;
    bool printPressed;
};
