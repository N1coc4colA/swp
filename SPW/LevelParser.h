#pragma once

#include "Settings.h"

class LevelScene;

class LevelParser
{
public:
    LevelParser(const std::string &path);
    ~LevelParser();

    void InitScene(LevelScene &scene) const;

    std::string themeName = "";
    
private:
    char **m_matrix;
    int m_width;
    int m_height;
};