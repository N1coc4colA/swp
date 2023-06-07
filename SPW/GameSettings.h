#pragma once

#include <SDL_scancode.h>

class GameSettings
{
public:
    explicit GameSettings();
    static GameSettings *get();

    SDL_Scancode jump,
                 left,
                 right,
                 special,
                 down,
                 pause;

    float soundGlobal,
          soundCollectable,
          soundEnemy,
          soundPlayer,
          soundSystem1,
          soundSystem2;

    void save() const;
    void load();
};
