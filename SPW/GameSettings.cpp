#include "GameSettings.h"

#include <fstream>
#include <iostream>


static GameSettings *settings = nullptr;


GameSettings::GameSettings()
{    
    settings = this;
}

GameSettings *GameSettings::get()
{
    return settings;
}

struct scancode
{
    unsigned int v : sizeof(SDL_Scancode);
};

std::istream &operator >>(std::istream& os, const SDL_Scancode &sc)
{
    os.read((char *)&sc, sizeof(SDL_Scancode));
    return os;
}

std::ostream &operator <<(std::ostream& os, const SDL_Scancode &sc)
{
    os.write((const char *)&sc, sizeof(SDL_Scancode));
    return os;
}

void GameSettings::load()
{
    std::ifstream input( "./settings.raw", std::ios::binary);
    if (input.is_open())
    {
        input
        >> jump
        >> left
        >> right
        >> down
        >> pause
        >> soundGlobal
        >> soundCollectable
        >> soundEnemy
        >> soundPlayer
        >> soundSystem1
        >> soundSystem2;
    } else
    {
        std::wcerr << "Unable to read settings!\n";
        jump = SDL_SCANCODE_SPACE;
        left = SDL_SCANCODE_LEFT;
        right = SDL_SCANCODE_RIGHT;
        down = SDL_SCANCODE_DOWN;
        pause = SDL_SCANCODE_S;
        soundGlobal = 0.5f;
        soundCollectable = 0.4f;
        soundEnemy = 0.5f;
        soundPlayer = 0.5f;
        soundSystem1 = 0.4f;
        soundSystem2 = 0.6f;

        save();
    }
}

void GameSettings::save() const
{
    std::ofstream output( "./settings.raw", std::ios::binary);
    if (output.is_open())
    {
        output
        << jump
        << left
        << right
        << down
        << pause
        << soundGlobal
        << soundCollectable
        << soundEnemy
        << soundPlayer
        << soundSystem1
        << soundSystem2;
    } else
    {
        std::wcerr << "Unabled to save settings!\n";
    }
}
