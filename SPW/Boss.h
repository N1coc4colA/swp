#pragma once

#include "Settings.h"
#include "Enemy.h"

class Boss
    : public Enemy
{
public:
    Boss(Scene &scene);
    ~Boss();

    void Start() override;
    void FixedUpdate() override;
    void Render() override;
    void OnRespawn() override;

    void Damage(GameBody *damager) override;

    void OnCollisionStay(GameCollision &collision) override;

    void Bounce(const PE_Vec2 &v);
    void Remove_life();
    int Get_life();
    void SetCloser(bool);
    bool GetCloser();


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
    int heart_count = 5;
    bool m_player_closer;
    int m_timer_shoot = 0;
    int m_timer_shield = 0;
};

inline int Boss::Get_life() {
    return heart_count;
}