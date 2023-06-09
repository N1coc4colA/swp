#pragma once

#include "Settings.h"
#include "Camera.h"
#include "Collectable.h"

class Portal : public Collectable
{
public:
    Portal(Scene& scene, int index);

    ~Portal();
    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void FixedUpdate() override;

    void OnCollisionEnter(GameCollision& collision) override;
    void Collect(GameBody* collector) override;
private:
    RE_Animator m_animator;
    bool m_respawn;
    int m_index;
    enum class State
    {
        IDLE,
        RUNNING
    };

    State m_state = State::IDLE;
};

