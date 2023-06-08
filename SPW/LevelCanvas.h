#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "PauseMenu.h"

class LevelScene;
class HubScene;

class LevelCanvas : public UIObject
{
public:
    LevelCanvas(LevelScene &scene);
    LevelCanvas(HubScene &scene);

    void Update() override;
    void OnRespawn() override;
    void Render() override;

    void OpenPauseMenu();
    void ClosePauseMenu();

private:
    PauseMenu *m_pauseMenu;
    LevelScene *m_levelScene;
    HubScene *m_hubScene;
};