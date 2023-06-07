#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "Text.h"

#include <functional>


class KeyButton : public UIObject
{
public:
    KeyButton(Scene &scene, RE_AtlasPart *atlasPart);
    ~KeyButton() override;

    enum class State : int
    {
        UP = 0,
        HOVER,
        DOWN,
        DISABLED
    };

    void SetButtonEnabled(bool enabled);
    bool IsButtonEnabled() const;
    void SetBorders(UIBorders *borders);

    void Update() override;
    void Render() override;
    void FixedUpdate() override;

    void SetText(const std::string &);
    void SetKey(SDL_Scancode sc);
    
    std::function<void (SDL_Scancode)> onChange = [](SDL_Scancode) {};

protected:
    void regenerateText();
    
    RE_AtlasPart *m_atlasPart;

    State m_currState;
    State m_prevState;

    UIBorders *m_borders;

    Text *usedKey = nullptr;
    Text *description = nullptr;
    
    std::string text = "KB";
    bool listening = false;
    SDL_Scancode key;
};

inline void KeyButton::SetBorders(UIBorders *borders)
{
    if (m_borders) delete m_borders;
    m_borders = borders;
}

