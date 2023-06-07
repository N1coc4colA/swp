#include "SettingsScene.h"
#include "TitleScene.h"
#include "Button.h"
#include "Text.h"
#include "Slider.h"
#include "GameSettings.h"
#include "StartScreen.h"
#include "KeyButton.h"

#include <functional>


class WayHome : public ButtonListener
{
public:
    std::function<void()> pressed = []() {};
    void OnPress() override {pressed();}
};

SettingsScene::SettingsScene(TitleScene& scene)
    : UIObject(scene)
{
    scene.GetInputManager().GetControls().SetEnabled(true);
    
    float buttonH = 55.0f;
    float topSkip = 800.0f;
    float sep = 10.0f;
    float panelW = 350.0f;
    float panelH = topSkip + 4.0f * buttonH + 3.0f * sep;

    m_rect.anchorMin.Set(0.5f, 0.5f);
    m_rect.anchorMax.Set(0.5f, 0.5f);
    m_rect.offsetMin.Set(-0.5f * panelW, -0.5f * panelH);
    m_rect.offsetMax.Set(+0.5f * panelW, +0.5f * panelH);

    AssetManager &assets = scene.GetAssetManager();
    RE_Atlas *atlas = assets.GetAtlas(AtlasID::UI);
    AssertNew(atlas)
    
    // Création du titre
    TTF_Font *font = assets.GetFont(FontID::LARGE);
    Text *title = new Text(scene, u8"Paramètres", font, assets.GetColor(ColorID::NORMAL));
    title->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    title->GetLocalRect().anchorMax.Set(1.0f, 0.0f);
    title->GetLocalRect().offsetMin.Set(0.0f, 0);
    title->GetLocalRect().offsetMax.Set(0.0f, topSkip);
    title->SetAnchor(RE_Anchor::NORTH);
    title->SetParent(this);

    // Création des boutons
    RE_AtlasPart *buttonPart = atlas->GetPart("Button");
    AssertNew(buttonPart);
    SDL_Color colorUp = assets.GetColor(ColorID::NORMAL);
    SDL_Color colorHover = assets.GetColor(ColorID::BLACK);
    SDL_Color colorDown = assets.GetColor(ColorID::NORMAL);
    font = assets.GetFont(FontID::NORMAL);

    
    const char *sliderTexts[6] = {
        "Son global",
        "Son collectables",
        "Son ennemis",
        "Son joueur",
        "Son controles 1",
        "Son controles 2"
    };

    std::function<void (float)> sliderSetters[6] = {
        [](float v)
        {
            GameSettings::get()->soundGlobal = v;
            Mix_VolumeMusic((int)(v * MIX_MAX_VOLUME));
        },
        [](float v)
        {
            GameSettings::get()->soundCollectable = v;
            Mix_Volume((int)ChannelID::COLLECTABLE, (int)(v * MIX_MAX_VOLUME));
        },
        [](float v)
        {
            GameSettings::get()->soundEnemy = v;
            Mix_Volume((int)ChannelID::ENEMY, (int)(v * MIX_MAX_VOLUME));
        },
        [](float v)
        {
            GameSettings::get()->soundPlayer = v;
            Mix_Volume((int)ChannelID::PLAYER, (int)(v * MIX_MAX_VOLUME));
        },
        [](float v)
        {
            GameSettings::get()->soundSystem1 = v;
            Mix_Volume((int)ChannelID::SYSTEM_1, (int)(v * MIX_MAX_VOLUME));
        },
        [](float v)
        {
            GameSettings::get()->soundSystem2 = v;
            Mix_Volume((int)ChannelID::SYSTEM_2, (int)(v * MIX_MAX_VOLUME));
        },
    };

    float sliderValues[6] = {
        GameSettings::get()->soundGlobal,
        GameSettings::get()->soundCollectable,
        GameSettings::get()->soundEnemy,
        GameSettings::get()->soundPlayer,
        GameSettings::get()->soundSystem1,
        GameSettings::get()->soundSystem2,
    };
    
    std::function<void (SDL_Scancode)> keySetters[7] = {
        [](SDL_Scancode v)
        {
            GameSettings::get()->shield = v;
        },
        [](SDL_Scancode v)
        {
            GameSettings::get()->jump = v;
        },
        [](SDL_Scancode v)
        {
            GameSettings::get()->left = v;
        },
        [](SDL_Scancode v)
        {
            GameSettings::get()->right = v;
        },
        [](SDL_Scancode v)
        {
            GameSettings::get()->special = v;
        },
        [](SDL_Scancode v)
        {
            GameSettings::get()->down = v;
        },
        [](SDL_Scancode v)
        {
            GameSettings::get()->pause = v;
        },
    };
    
    SDL_Scancode keyValues[7] = {
        GameSettings::get()->shield,
        GameSettings::get()->jump,
        GameSettings::get()->left,
        GameSettings::get()->right,
        GameSettings::get()->special,
        GameSettings::get()->down,
        GameSettings::get()->pause,
    };

    const char *keyDescriptions[7] = {
        "Shield",
        "Saut",
        "Gauche",
        "Droite",
        "Speciale",
        "Base",
        "Pause"
    };

    float curY = 100.f;
    int i;
    for (i = 0; i < 6; i++, curY += buttonH + sep)
    {
        Slider *slider = new Slider(scene, buttonPart);
        slider->SetValue(sliderValues[i]);
        slider->GetLocalRect().anchorMin.Set(0.f, 0.f);
        slider->GetLocalRect().anchorMax.Set(1.f, 0.f);
        slider->GetLocalRect().offsetMin.Set(0.f, curY);
        slider->GetLocalRect().offsetMax.Set(0.f, curY + buttonH);
        slider->SetParent(this);
        slider->SetBorders(new UIBorders(25, 25, 25, 25));
        auto listener = new SliderListener();
        listener->setter = sliderSetters[i];
        slider->SetListener(listener);

        Text *sliderLabel = new Text(scene, sliderTexts[i], font, colorHover);
        slider->SetText(sliderLabel, Slider::State::UP);

        sliderLabel = new Text(scene, sliderTexts[i], font, colorHover);
        slider->SetText(sliderLabel, Slider::State::HOVER);

        sliderLabel = new Text(scene, sliderTexts[i], font, colorHover);
        slider->SetText(sliderLabel, Slider::State::DOWN);
    }

    for (i = 0; i < 7; i++, curY += buttonH + sep)
    {
        KeyButton *kb = new KeyButton(scene, buttonPart);
        kb->GetLocalRect().anchorMin.Set(0.f, 0.f);
        kb->GetLocalRect().anchorMax.Set(1.f, 0.f);
        kb->GetLocalRect().offsetMin.Set(0.f, curY);
        kb->GetLocalRect().offsetMax.Set(0.f, curY + buttonH);
        kb->SetParent(this);
        kb->SetBorders(new UIBorders(25, 25, 25, 25));

        kb->SetText(keyDescriptions[i]);
        kb->SetKey(keyValues[i]);
        kb->onChange = keySetters[i];
    }
    
    Button *button = new Button(scene, buttonPart);
    button->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    button->GetLocalRect().anchorMax.Set(1.0f, 0.0f);
    button->GetLocalRect().offsetMin.Set(0.0f, curY);
    button->GetLocalRect().offsetMax.Set(-5.0f, curY + buttonH);
    button->SetParent(this);
    button->SetBorders(new UIBorders(25, 25, 25, 25));
    auto wh = new WayHome();
    wh->pressed = [this, &scene]()
    {
        StartScreen *sc = new StartScreen(scene);
        sc->SetParent(GetParent());
        Delete();
    };
    button->SetListener(wh);

    Text *buttonLabel = new Text(scene, "Retour", font, colorUp);
    button->SetText(buttonLabel, Button::State::UP);

    buttonLabel = new Text(scene, "Retour", font, colorHover);
    button->SetText(buttonLabel, Button::State::HOVER);

    buttonLabel = new Text(scene, "Retour", font, colorDown);
    button->SetText(buttonLabel, Button::State::DOWN);
}
