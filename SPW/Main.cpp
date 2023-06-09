#include "Settings.h"
#include "LevelScene.h"
#include "TitleScene.h"
#include "GameSettings.h"
#include "HubScene.h"

//#define FHD
//#define FULLSCREEN
//#define SKIP_MENU
//#define DEFAUT_LEVEL 0
//#define USE_BASIC_MENU


#ifdef FHD
#define WINDOW_WIDTH   FHD_WIDTH
#define WINDOW_HEIGHT  FHD_HEIGHT
#else
#define WINDOW_WIDTH   HD_WIDTH
#define WINDOW_HEIGHT  HD_HEIGHT
#endif

#define LOGICAL_WIDTH  FHD_WIDTH
#define LOGICAL_HEIGHT FHD_HEIGHT



enum class GameState
{
    MAIN_MENU,
	LEVEL,
    SELECTOR
};

int main(int argc, char *argv[])
{
    // Initialise la lib RE (SDL2, SDL2 image, SDL2 ttf)
    RE_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK, IMG_INIT_PNG);

    // Initialise la SDL2 mixer
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        std::cerr << "ERROR - Mix_Init " << Mix_GetError() << std::endl;
        assert(false);
        abort();
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        std::cerr << "ERROR - Mix_OpenAudio " << Mix_GetError() << std::endl;
        assert(false);
        abort();
    }
    Mix_AllocateChannels(16);
    
    // Loads the settings.
    GameSettings gSettings;
    gSettings.load();

    int exitStatus;
    exitStatus = Mix_MasterVolume((int)(MIX_MAX_VOLUME));
    //exitStatus = Mix_VolumeMusic((int)(0.70f * MIX_MAX_VOLUME));
    exitStatus = Mix_VolumeMusic((int)(gSettings.soundGlobal * MIX_MAX_VOLUME));
    exitStatus = Mix_Volume((int)ChannelID::COLLECTABLE, (int)(gSettings.soundCollectable * MIX_MAX_VOLUME));
    exitStatus = Mix_Volume((int)ChannelID::ENEMY, (int)(gSettings.soundEnemy * MIX_MAX_VOLUME));
    exitStatus = Mix_Volume((int)ChannelID::PLAYER, (int)(gSettings.soundPlayer * MIX_MAX_VOLUME));
    exitStatus = Mix_Volume((int)ChannelID::SYSTEM_1, (int)(gSettings.soundSystem1 * MIX_MAX_VOLUME));
    exitStatus = Mix_Volume((int)ChannelID::SYSTEM_2, (int)(gSettings.soundSystem2 * MIX_MAX_VOLUME));

    Mix_Music *music = Mix_LoadMUS("../Assets/Music/main.mp3");
    Mix_PlayMusic(music, -1);

    // Cr�e la fen�tre
    Uint32 sdlFlags = 0;
#ifdef FULLSCREEN
    sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

    SDL_Window *window = SDL_CreateWindow(
        u8"Super Potoo World ++", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, sdlFlags
    );

    if (!window)
    {
        std::cerr << "ERROR - Create window " << SDL_GetError() << std::endl;
        assert(false); abort();
    }

    // Cr�e le moteur de rendu
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (!renderer)
    {
        std::cerr << "ERROR - Create renderer " << SDL_GetError() << std::endl;
        assert(false); abort();
    }

    // V�rifie s'il y a un joystick
    SDL_Joystick *gameController = nullptr;
    if (SDL_NumJoysticks() >= 1)
    {
        gameController = SDL_JoystickOpen(0);
    }
    
    // Cr�e le temps global du jeu
    RE_Timer time;
    time.Start();

    // Boucle de jeu
    Scene *scene = nullptr;
    bool quitGame = false;
    GameState state = GameState::MAIN_MENU;
    std::vector<LevelData> levels(LevelData::Init());
    int levelID = 0;

//#ifdef SKIP_MENU
//    state = GameState::LEVEL;
//    levelID = DEFAUT_LEVEL;
//#endif

    while (quitGame == false)
    {
        time.SetTimeScale(1.0f);

        // Construction de la sc�ne
        switch (state)
        {
#ifndef USE_BASIC_MENU
        case GameState::SELECTOR:
            scene = new HubScene(renderer, time);
            break;
            
        case GameState::LEVEL:
            if (Scene::usesTrick)
            {
                levelID = Scene::tricked;
            }
            if (levelID == 100)
            {
                scene = new TitleScene(renderer, time, levels);
                state = GameState::MAIN_MENU;
            } else {
                assert(0 <= levelID && levelID < (int)levels.size());
            
#else
            case GameState::SELECTOR:
        
            case GameState::LEVEL:
                if (levelID == 100)
                {
                    levelID = 1;
                }
        assert(0 <= levelID && levelID < (int)levels.size());
#endif
                scene = new LevelScene(renderer, time, levels[levelID]);
            }
            break;

        case GameState::MAIN_MENU:
        default:
            scene = new TitleScene(renderer, time, levels);
            break;
        }

        // Boucle de rendu
        while (true)
        {
            // Met � jour le temps
            time.Update();

            // Met � jour la sc�ne
            bool quit = scene->Update();

            if (scene->GetInputManager().GetApplication().quitPressed)
            {
                quitGame = true;
                break;
            }
            if (quit)
            {
                break;
            }

            // Efface le rendu pr�c�dent
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Dessine la sc�ne
            scene->Render();

            // Affiche le nouveau rendu
            SDL_RenderPresent(renderer);
        }

        switch (state)
        {
        case GameState::LEVEL:
#ifndef USE_BASIC_MENU
            state = GameState::MAIN_MENU;
#else
            state = GameState::MAIN_MENU;
#endif
            break;

        case GameState::SELECTOR:
            state = scene->requiresLevelLoad ? GameState::LEVEL : GameState::MAIN_MENU;
            levelID = Scene::usesTrick ? Scene::tricked : levelID;
            break;
            
        case GameState::MAIN_MENU:
        default:
            levelID = ((TitleScene *)scene)->GetLevelID();
            if (levelID < 0)
            {
                quitGame = true;
            }
#ifndef USE_BASIC_MENU
            state = (levelID == 100) ? GameState::SELECTOR : GameState::LEVEL;
#else
            state = GameState::LEVEL;
#endif
            break;
        }

        if (scene)
        {
            delete scene;
            scene = nullptr;
        }
        scene->usesTrick = false;
    }

    gSettings.save();

    if (gameController)
    {
        SDL_JoystickClose(gameController);
        gameController = nullptr;
    }
    if (music)
    {
        Mix_FreeMusic(music);
        music = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    Mix_Quit();
    RE_Quit();

    return 0;
}
