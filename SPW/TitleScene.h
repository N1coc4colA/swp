#pragma once

#include "Settings.h"
#include "Scene.h"
#include "TitleCanvas.h"
#include "LevelData.h"

class TitleScene : public Scene
{
public:
    TitleScene(SDL_Renderer *renderer, RE_Timer &mainTime, const std::vector<LevelData> &levels);
    ~TitleScene() override;

    const std::vector<LevelData> &GetLevels() const;

private:
    TitleCanvas *m_canvas;
    const std::vector<LevelData> &m_levels;
};

inline const std::vector<LevelData>& TitleScene::GetLevels() const
{
    return m_levels;
}
