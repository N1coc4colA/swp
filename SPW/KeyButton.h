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

    void setText(const std::string &);
    std::function<void (SDL_Scancode)> onChange = [](SDL_Scancode) {};

protected:
    RE_AtlasPart *m_atlasPart;

    State m_currState;
    State m_prevState;

    UIBorders *m_borders;

    Text *usedKey = nullptr;
    std::string text = "KB";
    bool requiresReload = false;
};

inline void KeyButton::setText(const std::string &str)
{
    text = str;
    requiresReload = true;
}
