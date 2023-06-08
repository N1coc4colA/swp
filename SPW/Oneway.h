#pragma once

#include "Settings.h"
#include "Enemy.h"

class Oneway
    : public Enemy
{
public:
    Oneway(Scene &scene, int size,PE_Vec2 pos,int sens);
    ~Oneway();

    void Start() override;
    void FixedUpdate() override;
    void Render() override;
    void OnRespawn() override;

    

    

private:
    RE_Animator m_animator;

    enum class State
    {
        HEAD,
        BODY,
        HOUSE
    };
    int m_fixedCount;
    State m_state = State::HOUSE;
    bool m_isBounced = false;
    PE_Vec2 m_bounce;
    PE_Collider *m_collider;
    Oneway *m_oneways;
    PE_Vec2 m_posmax;
    PE_Vec2 m_posstart;
    bool m_retour;
    int m_size;
    int m_sens;
    PE_Vec2 position;
};