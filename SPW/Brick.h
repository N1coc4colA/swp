#pragma once

#include "Settings.h"
#include "GameBody.h"

class Brick : public GameBody
{
public:
    Brick(Scene &scene);
    ~Brick() override = default;
    
    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void OnCollisionStay(GameCollision &collision) override;
    void FixedUpdate() override;

    void touchedFromBottom();
    
private:
    RE_Animator m_animator;
    bool m_active = true;

    friend class Player;
};