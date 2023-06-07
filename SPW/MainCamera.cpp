#include "MainCamera.h"
#include "LevelScene.h"

#include <iostream>

MainCamera::MainCamera(LevelScene &scene) :
    Camera(scene), m_center()
{
    m_name = "MainCamera";

    m_center.SetSmoothTime(0.15f);
    m_center.SetMaxSpeed(1000.0f);
}

MainCamera::~MainCamera()
{
}

void MainCamera::Update()
{
    LevelScene &levelScene = dynamic_cast<LevelScene &>(m_scene);
    PE_Body *playerBody = levelScene.GetPlayer()->GetBody();

    m_center.SetTarget(playerBody->GetPosition());
    m_center.Update(levelScene.GetTime());

    PE_Vec2 shift(-3.0f, 0.0f);
    TranslateWorldView(m_center - m_worldView.GetCenter() - shift);
    
    // Fixe la position verticale minimale de la caméra pour ne pas voir sous le sol
    const float dispLowerY = m_worldBounds.lower.y - m_worldView.lower.y;
    const float dispUpperY = m_worldView.upper.y - m_worldBounds.upper.y;
    const float dispLeftX = m_worldBounds.lower.x - m_worldView.lower.x;
    const float dispRightX = m_worldView.upper.x - m_worldBounds.upper.x;
    if (dispLowerY > 0.f)
    {
        TranslateWorldView(PE_Vec2(0.f, dispLowerY));
    } else if (dispUpperY > 0.f)
    {
        TranslateWorldView(PE_Vec2(0.f, -dispUpperY));
    }
    if (dispLeftX > 0.f)
    {
        TranslateWorldView(PE_Vec2(dispLeftX, 0.f));
    } else if (dispRightX > 0.f)
    {
        TranslateWorldView(PE_Vec2(-dispRightX, 0.f));
    }
}
