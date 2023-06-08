#pragma once

#include "Settings.h"
#include "GameBody.h"

class FallingBlock : public GameBody
{
public:
    FallingBlock(Scene &scene);
    ~FallingBlock() override = default;
    
    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void OnCollisionStay(GameCollision &collision) override;
    void FixedUpdate() override;

    void touchedFromBottom();
    
private:
    RE_Animator m_animator;
    bool m_active = true;
    
    float m_timer = 0.f;
    bool m_timerRunning = false;

    friend class Player;
};