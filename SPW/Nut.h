#pragma once

#include "Settings.h"
#include "Enemy.h"

class Nut
    : public Enemy
{
public:
    Nut(Scene &scene);
    ~Nut();

    void Start() override;
    void FixedUpdate() override;
    void Render() override;
    void OnRespawn() override;

    void Damage(GameBody *damager) override;

    void OnCollisionStay(GameCollision &collision) override;

    void Bounce(const PE_Vec2 &v);

private:
    RE_Animator m_animator;

    enum class State
    {
        IDLE,
		SPINNING,
        JUMPED,
		DYING
    };

    State m_state = State::IDLE;
    bool m_isBounced = false;
    PE_Vec2 m_bounce;
};