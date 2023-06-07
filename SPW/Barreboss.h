#pragma once
#include "Settings.h"
#include "UIObject.h"
#include "Boss.h"
#include "LevelScene.h"

class Barreboss : public UIObject
{
public:
    Barreboss(Scene &scene, RE_AtlasPart *atlasPart, int index, LevelScene* lvlscene);

    void SetBorders(UIBorders *borders);

    void GetNaturalSize(int &width, int &height) const;
    float GetNaturalRatio() const;

    void Render() override;
    void Update() override;
    void FixedUpdate() override;


private:
    SDL_Texture *m_texture;
    SDL_Rect m_srcRect;
    UIBorders *m_borders;
    int m_index;
    int life_boss;
    int life_boss_start;
    bool m_player_closer;
    LevelScene *m_levelScene;
};

inline void Barreboss::SetBorders(UIBorders *borders)
{
    if (m_borders) delete m_borders;
    m_borders = borders;
}

inline void Barreboss::GetNaturalSize(int &width, int &height) const
{
    width = m_srcRect.w;
    height = m_srcRect.h;
}

inline float Barreboss::GetNaturalRatio() const
{
    return (float)m_srcRect.w / (float)m_srcRect.h;
}



inline void Barreboss::Update()
{
    SetVisible(true);
    
}

