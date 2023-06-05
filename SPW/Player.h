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

private:
    void WakeUpSurroundings();

    enum class State
    {
        IDLE,
		WALKING,
		RUNNING,
		SKIDDING,
		FALLING,
		DYING,
		DEAD
    };

    State m_state = State::IDLE;

    RE_Animator m_animator;

    float m_hDirection = 0.f;
    bool m_jump = false;
    bool m_onGround;
    bool m_bounce = false;
    bool m_facingRight = true;

    int m_lifeCount = 5;
    int m_heartCount = 2;
    int m_fireflyCount = 0;
	int m_consecutiveJumps = 0;
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
