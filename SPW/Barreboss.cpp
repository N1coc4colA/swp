#include "Barreboss.h"
#include "LevelScene.h"
Barreboss::Barreboss(Scene &scene, RE_AtlasPart *atlasPart, int index, LevelScene *lvlscene) :
    UIObject(scene), m_texture(nullptr), m_borders(nullptr), m_index(0), m_srcRect()
{
    m_name = "Barreboss";

    m_srcRect = *atlasPart->GetSrcRect(index);
    m_texture = atlasPart->GetTexture();
    m_levelScene = lvlscene;
    Boss* boss = m_levelScene->GetBoss();
    life_boss_start = boss->Get_life();
}


void Barreboss::FixedUpdate() {
    Boss* boss = m_levelScene->GetBoss();
    life_boss = boss->Get_life();

    m_player_closer = boss->GetCloser();
}

void Barreboss::Render()
{
    SDL_Renderer* renderer = m_scene.GetRenderer();
    
    if (m_texture)
    {
        if (m_player_closer) {
            SDL_FRect dstRect = GetCanvasRect();
            dstRect.w /= life_boss_start;
            for (int i = 0; i != life_boss; i++) {
                SDL_RenderCopyF(renderer, m_texture, &m_srcRect, &dstRect);
                dstRect.x -= dstRect.w;
            }
        }
    }

}