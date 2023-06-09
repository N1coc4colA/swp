#pragma once

#include "Settings.h"
#include "UIObject.h"

class LevelScene;
class HubScene;

class PauseMenu : public UIObject
{
public:
    PauseMenu(LevelScene &scene);
    PauseMenu(HubScene &scene);


    void Update() override;
};