#pragma once

#include "Settings.h"
#include "Enemy.h"

class Snake
    : public Enemy
{
public:
    Snake(Scene &scene, int size,PE_Vec2 pos,int sens);
    ~Snake();

    void Start() override;
    void FixedUpdate() override;
    void Render() override;
    void OnRespawn() override;

    void Damage(GameBody *damager) override;

    void OnCollisionStay(GameCollision &collision) override;

    

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
    PE_Vec2 m_posmax;
    PE_Vec2 m_posstart;
    bool m_retour;
    int m_size;
    int m_sens;
};