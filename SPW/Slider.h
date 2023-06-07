#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "Text.h"

#include <functional>


class SliderListener;

class Slider : public UIObject
{
public:
    Slider(Scene &scene, RE_AtlasPart *atlasPart);
    ~Slider() override;

    enum class State : int
    {
        UP = 0,
        HOVER,
        DOWN,
        DISABLED
    };

    void SetListener(SliderListener *listener);
    void SetSliderEnabled(bool enabled);
    bool IsSliderEnabled() const;
    void SetBorders(UIBorders *borders);

    void SetText(Text *text, State state);

    void Update() override;
    void Render() override;

    float value() const;
    void SetValue(float f);

protected:
    SliderListener *m_listener = nullptr;
    RE_AtlasPart *m_atlasPart;

    State m_currState = State::UP;
    State m_prevState = State::UP;

    UIBorders *m_borders = nullptr;
    std::array<Text *, 4> m_texts = std::array<Text *, 4>();

    float m_value = 1.f;
};

inline float Slider::value() const
{
    return m_value;
}


class SliderListener
{
public:
    SliderListener() = default;
    virtual ~SliderListener() = default;

    std::function<void (float)> setter = [](float) {};
    
    inline virtual void OnStateChange(Slider::State state) {}
    inline virtual void OnValueChange(float v) {setter(v);}
};

inline void Slider::SetListener(SliderListener * listener)
{
    m_listener = listener;
}

inline bool Slider::IsSliderEnabled() const
{
    return m_currState != Slider::State::DISABLED;
}

inline void Slider::SetBorders(UIBorders *borders)
{
    if (m_borders)
    {
        delete m_borders;
    }
    m_borders = borders;
}
