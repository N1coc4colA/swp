#include "KeyButton.h"
#include "Text.h"


KeyButton::KeyButton(Scene &scene, RE_AtlasPart *atlasPart) :
    UIObject(scene), m_atlasPart(atlasPart),
    m_currState(State::UP), m_prevState(State::UP),
    m_borders(nullptr)
{
    m_name = "KeyButton";
}

KeyButton::~KeyButton()
{
}

void KeyButton::SetButtonEnabled(bool enabled)
{
    if (enabled == (m_currState != State::DISABLED))
    {
        // Le boutton est déjà dans l'état souhaité
        return;
    }

    m_prevState = m_currState;
    m_currState = enabled ? State::UP : State::DISABLED;
}

void KeyButton::Update()
{
    SetVisible(true);

    MouseInput &mouse = m_scene.GetInputManager().GetMouse();
    ControlsInput &kbInput = m_scene.GetInputManager().GetControls();

    if (listening && kbInput.current != SDL_SCANCODE_UNKNOWN)
    {
        key = kbInput.current;
        onChange(key);
        listening = false;
        kbInput.disableDirectStroke = false;
        regenerateText();
    }
    
    m_prevState = m_currState;

    if (m_currState != State::DISABLED)
    {
        SDL_FPoint mousePos = { mouse.viewPos.x, mouse.viewPos.y };

        if (Contains(mousePos))
        {
            if (mouse.leftDown)
            {
                m_currState = State::DOWN;
            }
            else if (mouse.leftReleased)
            {
                m_scene.GetAssetManager().PlaySound(
                    SoundID::SYSTEM_SELECT,
                    ChannelID::SYSTEM_1
                );

                // Evite le déclanchement de plusieurs boutons 
                mouse.leftReleased = false;

                //Now listen to the keys!
                if (listening)
                {
                    onChange(key);
                    listening = false;
                    kbInput.disableDirectStroke = false;
                    regenerateText();
                } else
                {
                    listening = true;
                    kbInput.disableDirectStroke = true;
                }

                m_currState = State::HOVER;
            }
            else
            {
                m_currState = State::HOVER;
            }
        }
        else
        {
            m_currState = State::UP;
            
            if (listening)
            {
                onChange(key);
                key = kbInput.current;
                listening = false;
                kbInput.disableDirectStroke = false;
                regenerateText();
            }
        }
    }

    if (m_currState != m_prevState)
    {
        if (m_currState == State::HOVER)
        {
            m_scene.GetAssetManager().PlaySound(
                SoundID::SYSTEM_HOVER,
                ChannelID::SYSTEM_2
            );
        }
    }

    // Met à jour les textes du bouton
    if (usedKey)
    {
        usedKey->SetEnabled(m_currState != State::DISABLED);
    }
    if (description)
    {
        description->SetEnabled(m_currState != State::DISABLED);
    }
}

void KeyButton::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    SDL_Texture *texture = nullptr;

    if (m_atlasPart)
    {
        texture = m_atlasPart->GetTexture();

        SDL_FRect dstRect = GetCanvasRect();
        const SDL_Rect *srcRect = nullptr;

        if (m_currState == State::DISABLED) {
            srcRect = m_atlasPart->GetSrcRect(3);
        } else {
            srcRect = m_atlasPart->GetSrcRect(2);
        }

        if (m_borders)
        {
            RE_RenderCopyBordersF(
                renderer, texture, srcRect, &dstRect, RE_Anchor::NORTH_WEST,
                m_borders->left, m_borders->right, m_borders->top, m_borders->bottom
            );
        }
        else
        {
            SDL_RenderCopyF(renderer, texture, srcRect, &dstRect);
        }
    }
}

void KeyButton::SetText(const std::string &str)
{
    text = str;
    regenerateText();
}

void KeyButton::regenerateText()
{
    if (description)
    {
        description->Delete();
    }
    if (usedKey)
    {
        usedKey->Delete();
    }

    std::cout << "Actual key: \"" << SDL_GetScancodeName(key) << "\"" << std::endl;

    if (!text.empty())
    {
        description = new Text(
            m_scene,
            text,
            m_scene.GetAssetManager().GetFont(FontID::NORMAL),
            m_scene.GetAssetManager().GetColor(ColorID::BLACK));
    } else
    {
        description = nullptr;
    }
    if (strlen(SDL_GetScancodeName(key)) > 0)
    {
        usedKey = new Text(
            m_scene,
            SDL_GetScancodeName(key),
            m_scene.GetAssetManager().GetFont(FontID::NORMAL),
            m_scene.GetAssetManager().GetColor(ColorID::BLACK));
    } else
    {
        usedKey = nullptr;
    }
}

void KeyButton::SetKey(SDL_Scancode sc)
{
    key = sc;
    regenerateText();
}


void KeyButton::FixedUpdate()
{
}
