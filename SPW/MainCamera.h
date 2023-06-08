#pragma once

#include "Settings.h"
#include "Camera.h"
#include "Utils.h"

class LevelScene;
class HubScene;

class MainCamera : public Camera
{
public:
    MainCamera(LevelScene &scene);
    MainCamera(HubScene &scene);
    virtual ~MainCamera();

    virtual void Update() override;

protected:
    DampedVec2 m_center;
};