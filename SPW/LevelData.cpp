#include "LevelData.h"

LevelData::LevelData(const std::string &nameIn, const std::string &pathIn, ThemeID themeIDIn) :
    name(nameIn), path(pathIn), themeID(themeIDIn)
{
}

std::vector<LevelData> LevelData::Init()
{
    std::vector<LevelData> data;

    data.push_back(LevelData(
        u8"Montagnes",
        u8"../Assets/Level/0_Demo.txt",
        ThemeID::MOUNTAINS
    ));
    data.push_back(LevelData(
        u8"Démo",
        u8"../Assets/Level/1_Mountains.txt",
        ThemeID::SKY
        ));
    data.push_back(LevelData(
        u8"Montagnes",
        u8"../Assets/Level/2_Desert.txt",
        ThemeID::MOUNTAINS
        ));
    data.push_back(LevelData(
        u8"Montagnes",
        u8"../Assets/Level/3_Lost.txt",
        ThemeID::MOUNTAINS
    ));
    return data;
}
