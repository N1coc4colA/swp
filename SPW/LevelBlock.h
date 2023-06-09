#pragma once

#include "Settings.h"
#include "GameBody.h"
#include  "StaticMap.h"


class LevelBlock : public GameBody
{
public:
    LevelBlock(Scene &scene, Tile::Type tileType);
    
    void Start() override;
    void Render() override;
    void OnRespawn() override;
    void OnCollisionStay(GameCollision &collision) override;
    void FixedUpdate() override;
    void DrawGizmos() override;

    void trigger();

    void SetId(int i);
    
private:
    Tile::Type m_tileType;
    RE_AtlasPart *m_part = nullptr;
    RE_AtlasPart *m_whatPart = nullptr;

    int m_id = 0;

    RE_Text *text = nullptr;
    bool showDescription = false;
};
