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
    
    m_prevState = m_currState;

    if (m_currState != State::DISABLED)
    {
        const SDL_FPoint mousePos = { mouse.viewPos.x, mouse.viewPos.y };

        if (Contains(mousePos))
        {
            if (listening && kbInput.current != SDL_SCANCODE_UNKNOWN)
            {
                key = kbInput.current;
                onChange(key);
                listening = false;
                kbInput.disableDirectStroke = false;
                regenerateText();
            }
            
            if (mouse.leftDown)
            {
                m_currState = State::DOWN;
                
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
            }
            else if (mouse.leftReleased)
            {
                m_scene.GetAssetManager().PlaySound(
                    SoundID::SYSTEM_SELECT,
                    ChannelID::SYSTEM_1
                );

                // Evite le déclanchement de plusieurs boutons 
                mouse.leftReleased = false;


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
    } else
    {
        if (listening)
        {
            onChange(key);
            key = kbInput.current;
            listening = false;
            kbInput.disableDirectStroke = false;
            regenerateText();
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

    if (m_atlasPart)
    {
        SDL_Texture *texture = m_atlasPart->GetTexture();

        const SDL_FRect dstRect = GetCanvasRect();
        const SDL_FRect borderRect = {dstRect.x + dstRect.w/2, dstRect.y, dstRect.w/2, dstRect.h};
        const SDL_FRect borderRectEnd = {dstRect.x + dstRect.w - dstRect.h, dstRect.y, dstRect.h, dstRect.h};
        const SDL_FRect borderRectBegin = {dstRect.x + dstRect.w/2, dstRect.y, dstRect.h, dstRect.h};
        const SDL_Rect *srcRect = nullptr;

        if (m_currState == State::DOWN || listening) {
            srcRect = m_atlasPart->GetSrcRect(1);
        } else {
            srcRect = m_atlasPart->GetSrcRect(2);
        }
        
        SDL_RenderCopyF(renderer, texture, m_atlasPart->GetSrcRect(1), &borderRect);
        SDL_RenderCopyF(renderer, texture, m_atlasPart->GetSrcRect(1), &borderRectBegin);
        SDL_RenderCopyF(renderer, texture, m_atlasPart->GetSrcRect(1), &borderRectEnd);

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

    if (!text.empty())
    {
        description = new Text(
            m_scene,
            text,
            m_scene.GetAssetManager().GetFont(FontID::NORMAL),
            m_scene.GetAssetManager().GetColor(ColorID::BLACK));
        description->SetParent(this);
        description->SetAnchor(RE_Anchor::WEST);
        description->GetLocalRect().offsetMin.Set(10.f, 0.f);
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
        usedKey->SetParent(this);
        usedKey->GetLocalRect().anchorMin.Set(0.75f, 0.5f);
        usedKey->GetLocalRect().anchorMax.Set(0.75f, 0.5f);
        usedKey->GetLocalRect().offsetMax.Set(-10.f, 0.f);
        usedKey->SetAnchor(RE_Anchor::CENTER);
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