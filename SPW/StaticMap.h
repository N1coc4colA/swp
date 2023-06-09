#pragma once

#include "Settings.h"
#include "GameBody.h"

struct Tile
{
    enum class Type : int
    {
        EMPTY,
        GROUND,
        WOOD,
        ONE_WAY,
        SPIKE,
        STEEP_SLOPE_L,
        STEEP_SLOPE_R,
        GENTLE_SLOPE_L1,
        GENTLE_SLOPE_L2,
        GENTLE_SLOPE_R1,
        GENTLE_SLOPE_R2,
        BRICK
    };
    Type type;
    int partIdx;
    PE_Collider *collider;
};

class StaticMap : public GameBody
{
public:
    StaticMap(Scene &scene, int width, int height);
    ~StaticMap() override;

    void SetTile(int x, int y, Tile::Type type);
    void InitTiles();
    void InitWorldTiles();

    void Render() override;
    void Start() override;
    void OnCollisionStay(GameCollision &collision) override;

    void setIsWorld(bool isWorld);

    void setupAssets();
    
private:
    RE_AtlasPart *m_brickPart;
    RE_AtlasPart *m_woodPart;
    RE_AtlasPart *m_oneWayPart;
    RE_AtlasPart *m_terrainPart;
    RE_AtlasPart *m_spikePart;

    Tile **m_tiles;
    int m_width;
    int m_height;

    bool m_isWorld = false;

    Tile::Type GetTileType(int x, int y) const;
    bool IsGround(int x, int y) const;
};

inline void StaticMap::setIsWorld(bool isWorld)
{
    m_isWorld = isWorld;
}

