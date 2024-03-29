#include "LevelParser.h"
#include "StaticMap.h"
#include "Brick.h"
#include "LevelScene.h"
#include "Checkpoint.h"
#include "Nut.h"
#include "LevelEnd.h"
#include "Bonus.h"
#include "Camera.h"
#include "Firefly.h"
#include "Heart.h"
#include "Shield.h"
#include "Snake.h"
#include "Boss.h"
#include "Oneway.h"
#include "Bullet.h"
#include "FallingBlock.h"
#include "HubScene.h"
#include "LevelBlock.h"
#include "Portal.h"


#include <fstream>


LevelParser::LevelParser(const std::string &path)
{
    // Split it by ending to get the file name.
    savePath = "." + path.substr(path.find_last_of('/')) + ".save";
    loadSave();
    
    FILE *levelFile = fopen(path.c_str(), "rb");
    AssertNew(levelFile)

    bool isValidChar[128] = { 0 };
    isValidChar['\n'] = true;
    for (int i = '!'; i < '~'; i++)
    {
        isValidChar[i] = true;
    }

    fseek(levelFile, 0, SEEK_END);
    long fileSize = ftell(levelFile);
    rewind(levelFile);

    char *buffer = new char[fileSize];
    fread(buffer, 1, fileSize, levelFile);
    fclose(levelFile);
    levelFile = nullptr;
    
    //Parse the theme name
    int i = 0;
    while (buffer[i] != '\n')
    {
        themeName += buffer[i];
        i++;
    }

    //Remove the "\n" from our string.
    themeName.pop_back();

    int height = 0;
    int width = 0;
    for (i = i+1; i < fileSize; i++)
    {
        char c = buffer[i];
        if (isValidChar[(unsigned int)c])
        {
            if (c == '\n')
            {
                height = 1;
                break;
            }
            else
            {
                width++;
            }
        }
        else
        {
            std::wcerr << "INFO - Invalid char (" << c << ") - (" << (unsigned int)c << ")" << std::endl;
        }
    }
    if (height == 0)
    {
        std::cerr << "ERROR - Incorrect level file" << std::endl;
        assert(false);
        abort();
    }
    int w = 0;
    for (i = i + 1; i < fileSize; ++i)
    {
        char c = buffer[i];
        if (isValidChar[(unsigned int)c])
        {
            if (c == '\n')
            {
                if (w == 0)
                {
                    break;
                }
                height++;

                if (w != width)
                {
                    std::cerr << "ERROR - Incorrect level file" << std::endl;
                    assert(false);
                    abort();
                }
                w = 0;
            }
            else
            {
                w++;
            }
        }
        else
        {
            std::wcerr << "INFO - Invalid char (" << c << ") - (" << (unsigned int)c << ")" << std::endl;
        }
    }

    // Initialisation du parser
    m_width = width;
    m_height = height;

    m_matrix = new char *[m_width];
    for (int x = 0; x < width; ++x)
    {
        m_matrix[x] = new char[height];
    }

    // Remplissage de la matrice du niveau
    int x = 0;
    int y = height - 1;
    for (i = static_cast<int>(themeName.size()) + 1; i < fileSize; ++i)
    {
        char c = buffer[i];
        if (isValidChar[(unsigned int)c])
        {
            if (c == '\n')
            {
                x = 0;
                y--;
                if (y < 0)
                {
                    break;
                }
            }
            else
            {
                m_matrix[x][y] = c;
                x++;
            }
        }
    }

    delete[] buffer;
}

LevelParser::~LevelParser()
{
    if (m_matrix)
    {
        for (int i = 0; i < m_width; ++i)
        {
            delete[] m_matrix[i];
        }
        delete[] m_matrix;
    }
}

void LevelParser::InitScene(HubScene &scene) const
{
    // Cr�e la TileMap
    StaticMap *map = new StaticMap(scene, m_width, m_height);

    // So we can restart at the beginning.
    const int chkP = (levelDone) ? -1 : lastCheckPoint;
    int checkPointCount = 0;
    
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            LevelBlock *lb;
            PE_Vec2 position((float)x, (float)y);
            switch (m_matrix[x][y])
            {
            case '#': //Walls
                map->SetTile(x, y, Tile::Type::GROUND);
                break;
            case 'W': //Ways/Pavements
                map->SetTile(x, y, Tile::Type::WOOD);
                break;
            case 'A':
                lb = new LevelBlock(scene, Tile::Type::GENTLE_SLOPE_L1);
                lb->SetStartPosition(position);
                lb->SetId(0);
                break;
            case 'B':
                lb = new LevelBlock(scene, Tile::Type::GENTLE_SLOPE_L2);
                lb->SetStartPosition(position);
                lb->SetId(1);
                break;
            case 'C':
                lb = new LevelBlock(scene, Tile::Type::GENTLE_SLOPE_R1);
                lb->SetStartPosition(position);
                lb->SetId(2);
                break;
            case 'D':
                lb = new LevelBlock(scene, Tile::Type::GENTLE_SLOPE_R2);
                lb->SetStartPosition(position);
                lb->SetId(3);
                break;
            case 'S':
            {
                Player *player = scene.GetPlayer();
                player->SetStartPosition(position);
                break;
            }
            default:
                break;
            }
        }
    }
    map->setIsWorld(true);
    map->setupAssets();
    map->InitTiles();

    const PE_AABB bounds(0.0f, 0.0f, (float)m_width, 24.0f * 9.0f / 16.0f);
    Camera *camera = scene.GetActiveCamera();
    camera->SetWorldBounds(bounds);
}

void LevelParser::InitScene(LevelScene &scene) const
{
    // Cr�e la TileMap
    StaticMap *map = new StaticMap(scene, m_width, m_height);

    // So we can restart at the beginning.
    const int chkP = (levelDone) ? -1 : lastCheckPoint;
    int checkPointCount = 0;
    
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            PE_Vec2 position((float)x, (float)y);
            switch (m_matrix[x][y])
            {
            case '#':
                map->SetTile(x, y, Tile::Type::GROUND);
                break;
            case 'W':
                map->SetTile(x, y, Tile::Type::WOOD);
                break;
            case '=':
                map->SetTile(x, y, Tile::Type::ONE_WAY);
                break;
            case 'A':
                map->SetTile(x, y, Tile::Type::SPIKE);
                break;
            case '\\':
                map->SetTile(x, y, Tile::Type::STEEP_SLOPE_L);
                break;
            case '/':
                map->SetTile(x, y, Tile::Type::STEEP_SLOPE_R);
                break;
            case 'L':
                map->SetTile(x, y, Tile::Type::GENTLE_SLOPE_L1);
                break;
            case 'l':
                map->SetTile(x, y, Tile::Type::GENTLE_SLOPE_L2);
                break;
            case 'r':
                map->SetTile(x, y, Tile::Type::GENTLE_SLOPE_R1);
                break;
            case 'R':
                map->SetTile(x, y, Tile::Type::GENTLE_SLOPE_R2);
                break;
            case 'S':
            {
                Player *player = scene.GetPlayer();
                player->SetStartPosition(position);
                break;
            }
            case 'F':
            {
                LevelEnd *levelEnd = new LevelEnd(scene);
                levelEnd->SetStartPosition(position);
                break;
            }
            case 'e':
            {
                Nut *nut = new Nut(scene);
                nut->SetStartPosition(position);
                break;
            }
            case 'E':
            {
                Boss* boss = new Boss(scene);
                boss->SetStartPosition(position);
                scene.SetBoss(boss);
                break;
            }
            case 'o':
            {
                Firefly* firefly = new Firefly(scene,true);
                firefly->SetStartPosition(position);
                break;
            }
            case 'O':
            {
                Oneway* oneway = new Oneway(scene, 1, position, -1);
                oneway->SetStartPosition(position);
                break;
            }
            case 'H':
            {
                Heart* heart = new Heart(scene,true);
                heart->SetStartPosition(position);
                break;
            }
            case 'P':
            {
                Bullet* bullet = new Bullet(scene);
                bullet->SetStartPosition(position);
                break;
            }
            case 's':
            {
                Shield* shield = new Shield(scene,true);
                shield->SetStartPosition(position);
                break;
            }
            case 'C':
            {
                Checkpoint* checkpoint = new Checkpoint(scene);
                checkpoint->SetStartPosition(position);
                checkpoint->m_id = checkPointCount;
                if (checkPointCount < chkP)
                {
                    checkpoint->empty = true;
                }
                else if (chkP == checkPointCount)
                {
                    //Set as current one.
                    scene.m_player->SetStartPosition(position + PE_Vec2{0.f, 2.f});
                    checkpoint->empty = true;
                }
                checkPointCount++;
                break;
            }
            case 'M':
            {
                Bonus* bonus = new Bonus(scene);
                bonus->SetStartPosition(position);
                break;
            }
            case 'b':
            {
                Brick *brick = new Brick(scene);
                brick->SetStartPosition(position);
                break;
            }
            case 'z':
            {
                
                scene.portal1 = new Portal(scene,1);
                scene.portal1->SetStartPosition(position);

                break;
            }
            case 'Z':
            {

                scene.portal2 = new Portal(scene,2);
                scene.portal2->SetStartPosition(position);

                break;
            }
            
            case '6':
            {
                Snake* snake = new Snake(scene,3,position,-1,Layer::ENEMYHOUSE);
                snake->SetStartPosition(position);
                
                break;
            }
            case '9':
            {
                Snake* snake = new Snake(scene, 3, position, 1, Layer::ENEMYHOUSE);
                snake->SetStartPosition(position);

                break;
            }
            case 'f':
            {
                FallingBlock *falling = new FallingBlock(scene);
                falling->SetStartPosition(position);

                break;
            }
            default:
                break;
            }
        }
    }
    map->setupAssets();
    map->InitTiles();

    const PE_AABB bounds(0.0f, 0.0f, (float)m_width, 24.0f * 9.0f / 16.0f);
    Camera *camera = scene.GetActiveCamera();
    camera->SetWorldBounds(bounds);
}

void LevelParser::loadSave()
{
    if (savePath.size() < 7)
    {
        std::wcerr << "Invalid save path: \"" << savePath.c_str() << "\"" << std::endl;
        return;
    }
    std::ifstream input(savePath, std::ios_base::in | std::ios_base::binary);
    if (input.is_open())
    {
        input >> levelDone >> lastCheckPoint;
    } else
    {
        std::wcerr << "Map has no save file." << std::endl;
        levelDone = false;
        lastCheckPoint = -1;
        saveSave(savePath, levelDone, lastCheckPoint);
    }
}

void LevelParser::saveSave(const std::string &savePath, bool finished, int lastCheckPointDone)
{
    if (savePath.size() < 7)
    {
        std::wcerr << "Invalid save path: \"" << savePath.c_str() << "\"" << std::endl;
        return;
    }
    std::ofstream output(savePath, std::ios_base::out | std::ios_base::binary);
    if (output.is_open())
    {
        output << finished << lastCheckPointDone;
    } else
    {
        std::wcerr << "Unable to save map at \"" << savePath.c_str() << "\"" << std::endl;
    }
}