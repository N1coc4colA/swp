#pragma once

#include "Settings.h"

class LevelScene;
class HubScene;

class LevelParser
{
public:
    LevelParser(const std::string &path);
    ~LevelParser();

    void InitScene(LevelScene &scene) const;
    void InitScene(HubScene &scene) const;

    std::string themeName;
    std::string savePath;
    int lastCheckPoint = 0;
    bool levelDone = false;

    static void saveSave(const std::string &path, bool finished, int lastCheckPointDone);
    
private:
    char **m_matrix;
    int m_width;
    int m_height;

    void loadSave();
};