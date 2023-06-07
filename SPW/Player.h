#pragma once

#include "Settings.h"
#include "GameBody.h"

#define PLAYER_DAMAGE_ANGLE 55.0f

class Player : public GameBody
{
public:
    Player(Scene &scene);
    virtual ~Player();

    void Start() override;
    void Update() override;
    void Render() override;
    void FixedUpdate() override;
    void OnRespawn() override;
    void DrawGizmos() override;

    void OnCollisionEnter(GameCollision &collision) override;
    void OnCollisionStay(GameCollision &collision) override;

    int GetFireflyCount() const;
    int GetHeartCount() const;
    int GetLifeCount() const;
    void AddFirefly(int count);
    void AddHeart();
    void Damage();
    void Kill();
    void Bounce();
    void SetShield(bool res);
    void StartTimerShield();
    bool GetShield();

private:
    void WakeUpSurroundings();

    enum class State
    {
        IDLE,
		WALKING,
        LEAP,
		RUNNING,
		SKIDDING,
		FALLING,
		DYING,
		DEAD
    };

    State m_state = State::IDLE;

    RE_Animator m_animator;
    RE_Animator m_shieldAanimator; //We need a second animator to overlay.

    float m_hDirection = 0.f;
    bool m_jump = false;
    bool m_onGround;
    bool m_bounce = false;
    bool m_facingRight = true;
    bool shield = false;
    int m_lifeCount = 3;
    int m_heartCount = 3;
    int m_heartstart = m_heartCount;
    int m_fireflyCount = 0;
	int m_consecutiveJumps = 0;
    float timer_shield = 0;
    bool timer_start = false;
    bool m_jumpedOnce = false;
    bool m_canDash = false;
    PE_Vec2 m_dashDirection;
    bool m_drifting = false;
   
    
};


inline void Player::Bounce()
{
    m_bounce = true;
}

inline int Player::GetFireflyCount() const
{
    return m_fireflyCount;
}

inline int Player::GetHeartCount() const
{
    return m_heartCount;
}

inline int Player::GetLifeCount() const
{
    return m_lifeCount;
}

inline void Player::SetShield(bool res) {
    shield = res;
}

inline bool Player::GetShield() {
    return shield;
}


inline void Player::StartTimerShield() {
    timer_start = true;
}

