#include "LevelScene.h"
#include "LevelParser.h"
#include "MainCamera.h"
#include "DebugCamera.h"
#include "Background.h"
#include "Checkpoint.h"



LevelScene::LevelScene(SDL_Renderer *renderer, RE_Timer &mainTime, LevelData &level)
    : Scene(renderer, mainTime, level.themeID)
    , m_cameras()
{
    m_inputManager.GetApplication().SetEnabled(true);
    m_inputManager.GetMouse().SetEnabled(true);
    m_inputManager.GetControls().SetEnabled(true);
    m_inputManager.GetDebug().SetEnabled(true);

    setLevelEnded = [this]() {this->m_levelEnded = true;};

    // Crée le Player
    m_player = new Player(*this);
    m_player->SetName("Player");

    // Crée les caméras
    m_cameras[0] = new MainCamera(*this);
    m_cameras[1] = new DebugCamera(*this, *m_cameras[0]);
    m_activeCam = m_cameras[m_camIndex];

    // Parse le niveau
    LevelParser parser(level.path);
    parser.InitScene(*this);
    m_levelSavePath = parser.savePath;

    // Canvas
    m_canvas = new LevelCanvas(*this);

    if (parser.themeName == "Lake")
    {
        level.themeID = ThemeID::LAKE;
        
        // Crée le fond
        Background *background = new Background(*this, Layer::BACKGROUND);
        std::vector<SDL_Texture*> m_textures = m_assetManager.GetBackgrounds(ThemeID::LAKE);
        
        worldDim = {24.0f, 24.0f * 1080.0f / 1920.0f };
        background->SetWorldDimensions(worldDim);
        float factors[] = { 0.0f, 0.05f, 0.3f, 0.6f, 0.7f };
        for (int i = 0; i < 5; i++)
        {
            background->SetTexture(i, m_textures[i], PE_Vec2(factors[i], factors[i]));
        }
    } else if (parser.themeName == "Mountains")
    {
        level.themeID = ThemeID::MOUNTAINS;
        
        // Crée le fond
        Background *background = new Background(*this, Layer::BACKGROUND);
        std::vector<SDL_Texture*> m_textures = m_assetManager.GetBackgrounds(ThemeID::MOUNTAINS);
        
        worldDim = { 36.0f, 36.0f * 1080.0f / 2880.0f };
        background->SetWorldDimensions(worldDim);
        float factors[] = { 0.0f, 0.05f, 0.3f, 0.6f };
        for (int i = 0; i < 4; i++)
        {
            background->SetTexture(i, m_textures[i], PE_Vec2(factors[i], factors[i]));
        }

        Background *foreground = new Background(*this, Layer::FOREGROUND);
        worldDim.Set(36.0f, 36.0f * 400.0f / 2880.0f);
        foreground->SetWorldDimensions(worldDim);
        foreground->SetTexture(0, m_textures[4], PE_Vec2(1.4f, 1.4f));
    } else if (parser.themeName == "Sky")
    {
        level.themeID = ThemeID::SKY;
        
        // Crée le fond
        Background *background = new Background(*this, Layer::BACKGROUND);
        std::vector<SDL_Texture*> m_textures = m_assetManager.GetBackgrounds(ThemeID::SKY);
        
        worldDim = { 24.0f, 24.0f * 1080.0f / 1920.0f };
        background->SetWorldDimensions(worldDim);
        float factors[] = { 0.0f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.7f };
        for (int i = 0; i < 7; i++)
        {
            background->SetTexture(i, m_textures[i], PE_Vec2(factors[i], factors[i]));
        }
    } else if (parser.themeName == "Lost")
    {
        level.themeID = ThemeID::LOST;
        
        // Crée le fond
        Background *background = new Background(*this, Layer::BACKGROUND);
        std::vector<SDL_Texture*> m_textures = m_assetManager.GetBackgrounds(ThemeID::LOST);
        
        worldDim = { 24.0f, 24.0f * 1080.0f / 1920.0f };
        background->SetWorldDimensions(worldDim);
        float factors[] = { 0.0f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.7f };
        for (int i = 0; i < 7; i++)
        {
            background->SetTexture(i, m_textures[i], PE_Vec2(factors[i], factors[i]));
        }
    } else if (parser.themeName == "Desert")
    {
        level.themeID = ThemeID::DESERT;
        
        // Crée le fond
        Background *background = new Background(*this, Layer::BACKGROUND);
        std::vector<SDL_Texture*> m_textures = m_assetManager.GetBackgrounds(ThemeID::DESERT);
        
        worldDim = { 24.0f, 24.0f * 1080.0f / 1920.0f };
        background->SetWorldDimensions(worldDim);
        float factors[] = { 0.0f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.7f };
        for (int i = 0; i < 7; i++)
        {
            background->SetTexture(i, m_textures[i], PE_Vec2(factors[i], factors[i]));
        }
    } else
    {
        std::cerr << "Unknown theme name \"" << parser.themeName << "\"" << std::endl;
        assert(false);
        abort();
    }
}

LevelScene::~LevelScene()
{
    // Now lookup and save!
    int maxSaved = -1;
    for (auto it : m_objectManager) {
        if (Checkpoint *chkP = dynamic_cast<Checkpoint *>(it))
        {
            if (chkP->empty && maxSaved < chkP->m_id)
            {
                maxSaved = chkP->m_id;
            }
        }
    }
    
    LevelParser::saveSave(m_levelSavePath, m_levelEnded, maxSaved);
}

inline void LevelScene::SetPaused(bool isPaused)
{
    if (isPaused == m_paused)
        return;

    if (isPaused)
    {
        m_time.SetTimeScale(0.0f);
        m_paused = true;
        m_inputManager.GetControls().SetEnabled(false);
        m_canvas->OpenPauseMenu();
    }
    else
    {
        m_time.SetTimeScale(1.0f);
        m_paused = false;
        m_inputManager.GetControls().SetEnabled(true);
        m_canvas->ClosePauseMenu();
    }
}

bool LevelScene::Update()
{
    // Appelle la méthode de la classe mère
    bool quit = Scene::Update();

    ApplicationInput &appInput = m_inputManager.GetApplication();

    if (appInput.quitPressed)
    {
        return true;
    }

    if (appInput.pausePressed)
    {
        SetPaused(!m_paused);
    }

    DebugInput &debugInput = m_inputManager.GetDebug();

    if (debugInput.bodyPressed)
    {
        m_drawPhysics = !(m_drawPhysics);
    }
    if (debugInput.gizmosPressed)
    {
        m_drawGizmos = !(m_drawGizmos);
    }
    if (debugInput.idPressed)
    {
        m_drawID = !(m_drawID);
    }
    if (debugInput.camPressed)
    {
        m_camIndex = (m_camIndex + 1) % 2;
        m_activeCam = m_cameras[m_camIndex];
    }


    if (m_paused == false)
    {
        if (m_mode != UpdateMode::STEP_BY_STEP && debugInput.nextStepPressed)
        {
            m_mode = UpdateMode::STEP_BY_STEP;
        }
        else if (debugInput.quitStepPressed)
        {
            m_mode = UpdateMode::REALTIME;
        }

        // Mode pas-à-pas
        if (m_mode == UpdateMode::STEP_BY_STEP)
        {
            if (debugInput.nextStepPressed)
            {
                m_stepDelay = 0.5f;
                m_makeStep = true;
            }
            else if (debugInput.nextStepDown)
            {
                m_stepDelay -= m_time.GetUnscaledDelta();
            }

            if (m_stepDelay < 0.0f)
            {
                m_makeStep = true;
                m_stepDelay = 1.0f / 10.0f;
            }
        }
    }

    return quit;
}

void LevelScene::OnRespawn()
{
    SetPaused(false);
}
