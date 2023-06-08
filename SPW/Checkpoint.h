#pragma once

#include "Settings.h"
#include "GameBody.h"

class Checkpoint : public GameBody
{
public:
    Checkpoint(Scene &scene);

    void Start() override;
    void Render() override;
    void OnCollisionEnter(GameCollision &collision) override;

private:
    RE_Animator m_animator;
    bool m_isActive;
    bool empty;

    friend class LevelParser;
};