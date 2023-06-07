#pragma once

#include "Settings.h"
#include "GameBody.h"

class Bonus : public GameBody
{
public:
    Bonus(Scene &scene);

    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void OnCollisionEnter(GameCollision &collision) override;
    void Give_Bonus();
    void Set_BonusEmpty();

private:
    RE_Animator m_animator;
    bool m_hit;
    bool m_active = true;
    enum class State
    {
        FULL,
        EMPTY
    };

    State m_state = State::FULL;

};