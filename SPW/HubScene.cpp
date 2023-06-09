#include "HubScene.h"
#include "LevelParser.h"
#include "MainCamera.h"
#include "DebugCamera.h"
#include "Background.h"
#include "HubPlayer.h"


HubScene::HubScene(SDL_Renderer *renderer, RE_Timer &mainTime)
    : Scene(renderer, mainTime, ThemeID::WORLD)
    , m_cameras()
{
    m_inputManager.GetApplication().SetEnabled(true);
    m_inputManager.GetMouse().SetEnabled(true);
    m_inputManager.GetControls().SetEnabled(true);
    m_inputManager.GetDebug().SetEnabled(true);

    // Crée le Player
    m_player = new HubPlayer(*this);
    m_player->SetName("Player");

    // Crée les caméras
    m_cameras[0] = new MainCamera(*this);
    m_cameras[1] = new DebugCamera(*this, *m_cameras[0]);
    m_activeCam = m_cameras[m_camIndex];

    // Parse le niveau
    LevelParser parser("../Assets/Level/world.txt");
    parser.InitScene(*this);
    m_levelSavePath = parser.savePath;

    // Canvas
    m_canvas = new LevelCanvas(*this);

        
    // Crée le fond
    Background *background = new Background(*this, Layer::BACKGROUND);
    std::vector<SDL_Texture*> m_textures = m_assetManager.GetBackgrounds(ThemeID::WORLD);
    
    worldDim = { 24.0f, 24.0f * 1080.0f / 1920.0f };
    background->SetWorldDimensions(worldDim);
    background->SetTexture(0, m_textures[0], PE_Vec2::zero);
}

void HubScene::SetPaused(bool isPaused)
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

bool HubScene::Update()
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

void HubScene::OnRespawn()
{
    SetPaused(false);
}
