#pragma once

#include "Settings.h"
#include "Scene.h"
#include "Player.h"
#include "LevelCanvas.h"
#include "LevelData.h"

class Boss;

class LevelScene : public Scene
{
public:
    LevelScene(SDL_Renderer *renderer, RE_Timer &mainTime, const LevelData &level);
    LevelScene(LevelScene const&) = delete;
    LevelScene& operator=(LevelScene const&) = delete;
    virtual ~LevelScene();

    virtual bool Update() override;
    virtual void OnRespawn() override;

    Player *GetPlayer() const;
    Boss* GetBoss() const;
    void SetBoss(Boss*);
    bool IsPaused() const;
    void SetPaused(bool isPaused);

    PE_Vec2 worldDim;

private:
    std::array<Camera *, 2> m_cameras;
    Player *m_player;
    Boss* m_boss = nullptr;
    LevelCanvas *m_canvas;

    PE_Vec2 m_startPos;

    bool m_paused = false;
    int m_camIndex = 0;
    float m_stepDelay = 0.f;
};

inline Player *LevelScene::GetPlayer() const
{
    return m_player;
}

inline Boss* LevelScene::GetBoss() const
{
    return m_boss;
}
inline void LevelScene::SetBoss(Boss* MBoss) 
{
    m_boss = MBoss;
}

inline bool LevelScene::IsPaused() const
{
    return m_paused;
}
