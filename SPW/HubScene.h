#pragma once

#include "Settings.h"
#include "Scene.h"
#include "Player.h"
#include "LevelCanvas.h"
#include "LevelData.h"

class HubScene : public Scene
{
public:
    HubScene(SDL_Renderer *renderer, RE_Timer &mainTime);
    HubScene(HubScene const&) = delete;
    HubScene& operator=(HubScene const&) = delete;

    bool Update() override;
    void OnRespawn() override;

    Player *GetPlayer() const;
    bool IsPaused() const;
    void SetPaused(bool isPaused);

    PE_Vec2 worldDim;

    inline operator Scene *()
    {
        return this;
    }
    
private:
    std::array<Camera *, 2> m_cameras;
    Player *m_player;
    LevelCanvas *m_canvas;

    PE_Vec2 m_startPos;

    bool m_paused = false;
    int m_camIndex = 0;
    float m_stepDelay = 0.f;

    std::string m_levelSavePath;

    bool m_levelEnded = false;

    friend class LevelParser;
};

inline Player *HubScene::GetPlayer() const
{
    return m_player;
}

inline bool HubScene::IsPaused() const
{
    return m_paused;
}
