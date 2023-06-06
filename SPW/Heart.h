#pragma once

#include "Settings.h"
#include "Camera.h"
#include "Collectable.h"

class Heart : public Collectable
{
public:
    Heart(Scene &scene);
    
    ~Heart();
    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void FixedUpdate() override;

    void OnCollisionEnter(GameCollision &collision) override;
    void Collect(GameBody *collector) override;
private:
    RE_Animator m_animator;
    enum class State
    {
        IDLE,
        RUNNING
    };

    State m_state = State::IDLE;
};

