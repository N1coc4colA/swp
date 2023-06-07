#pragma once

#include "Settings.h"
#include "Enemy.h"

class Snake
    : public Enemy
{
public:
    Snake(Scene &scene);
    ~Snake();

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
		DASH,
        IN,
		DYING
    };

    State m_state = State::IDLE;
    bool m_isBounced = false;
    PE_Vec2 m_bounce;
    PE_Collider *m_collider;
};