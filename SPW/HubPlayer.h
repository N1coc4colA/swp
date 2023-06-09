#pragma once

#include "Player.h"

#ifndef PLAYER_DAMAGE_ANGLE
#define PLAYER_DAMAGE_ANGLE 55.0f
#endif


class HubPlayer : public Player
{
public:
    HubPlayer(Scene &scene);

    void Start() override;
    void Update() override;
    void Render() override;
    void FixedUpdate() override;
    void OnRespawn() override;
    void DrawGizmos() override;

    void OnCollisionEnter(GameCollision &collision) override;
    void OnCollisionStay(GameCollision &collision) override;

private:
    enum Direction
    {
        NONE,
        UPWARD,
        DOWNWARD,
        FORWARD,
        BACKWARD
    };

    Direction m_direction;
    bool m_goingDown = false;
};