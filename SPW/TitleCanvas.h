#pragma once

#include "Settings.h"
#include "UIObject.h"

class TitleScene;

class TitleCanvas : public UIObject
{
public:
    TitleCanvas(TitleScene &scene);

    void Update() override;
    void Render() override;
};