#pragma once

#include "Collectable.h"
#include "Camera.h"
#include "Player.h"


class Firefly
    : public Collectable
{
public:
    Firefly(Scene &scene, bool respawn);
    ~Firefly();
    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void Collect(GameBody *collector) override;

private:
    RE_Animator m_animator;
    bool m_respawn;
};
