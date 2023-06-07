#include "ControlsInput.h"

#include "GameSettings.h"


ControlsInput::ControlsInput() :
    InputGroup(), hAxis(0.0f),
    jumpDown(false), jumpPressed(false), goDownDown(false)
{
}

void ControlsInput::OnPreEventProcess()
{
    jumpPressed = false;
}

void ControlsInput::OnEventProcess(SDL_Event evt)
{
    SDL_Scancode scanCode;
    switch (evt.type)
    {
    case SDL_JOYHATMOTION:
        switch (evt.jhat.value)
        {
        case SDL_HAT_CENTERED:
        case SDL_HAT_UP:
            hAxis = 0.0f;
            goDownDown = false;
            break;
        case SDL_HAT_RIGHT:
        case SDL_HAT_RIGHTUP:
        case SDL_HAT_RIGHTDOWN:
            hAxis = +1.0f;
            goDownDown = false;
            break;
        case SDL_HAT_LEFT:
        case SDL_HAT_LEFTUP:
        case SDL_HAT_LEFTDOWN:
            hAxis = -1.0f;
            goDownDown = false;
            break;
        case SDL_HAT_DOWN:
            goDownDown = true;
            break;
        default:
            break;
        }
        break;

    case SDL_JOYAXISMOTION:
        if (evt.jaxis.axis == 0)
        {
            // Joystick gauche : Axe X
            float value = (float)(evt.jaxis.value);
            if (fabsf(value) < 8000.0f)
                hAxis = 0.0f;
            else
                hAxis = value / 32768.0f;

        }
        break;

    case SDL_JOYBUTTONDOWN:
        switch (evt.jbutton.button)
        {
        case 0: case 2:
            // Saut
            jumpDown = true;
            jumpPressed = true;
            break;
        default:
            break;
        }
        break;

    case SDL_JOYBUTTONUP:
        switch (evt.jbutton.button)
        {
        case 0: case 2:
            // Saut
            jumpDown = false;
            break;
        default:
            break;
        }
        break;

    case SDL_KEYDOWN:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        if (scanCode == GameSettings::get()->down)
        {
            // Descente d'une plateforme
            goDownDown = true;
        } else if (scanCode == GameSettings::get()->right)
        {
            // Deplacement à droite
            hAxis = 1.f;
        } else if (scanCode == GameSettings::get()->left)
        {
            // Deplacement à gauche
            hAxis = -1.f;
        } else if (scanCode == GameSettings::get()->jump)
        {
            // Saut
            jumpDown = true;
            jumpPressed = true;
        } else if (scanCode == GameSettings::get()->special)
        {
            specialPressed = true;
        }
        break;

    case SDL_KEYUP:
        scanCode = evt.key.keysym.scancode;

        if (scanCode == GameSettings::get()->down)
        {
            // Descente d'une plateforme
            goDownDown = true;
        }
        else if (scanCode == GameSettings::get()->jump)
        {
            // Saut
            jumpDown = false;
        }
        else if (scanCode == GameSettings::get()->left)
        {
            // Déplacement à gauche
            if (hAxis < 0.f)
                hAxis = 0.f;
        }
        else if (scanCode == GameSettings::get()->right)
        {
            // Deplacement à droite
            if (hAxis > 0.f)
                hAxis = 0.f;
        }
        break;

    default:
        break;
    }
}

void ControlsInput::Reset()
{
    hAxis = 0.f;
    jumpDown = false;
    jumpPressed = false;
    goDownDown = false;
    specialPressed = false;
}
