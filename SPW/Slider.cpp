#include "Slider.h"

Slider::Slider(Scene &scene, RE_AtlasPart *atlasPart)
    : UIObject(scene)
    , m_atlasPart(atlasPart)
{
    m_name = "Slider";
    for (auto &text : m_texts)
    {
        text = nullptr;
    }
}

Slider::~Slider()
{
    if (m_listener)
    {
        delete m_listener;
    }
}

void Slider::SetSliderEnabled(bool enabled)
{
    if (enabled == (m_currState != State::DISABLED))
    {
        // Le boutton est déjà dans l'état souhaité
        return;
    }

    m_prevState = m_currState;
    m_currState = enabled ? State::UP : State::DISABLED;

    // Callback "OnStateChanged"
    if (m_listener)
    {
        m_listener->OnStateChange(m_currState);
    }
}

void Slider::Update()
{
    SetVisible(true);

    MouseInput &mouse = m_scene.GetInputManager().GetMouse();

    m_prevState = m_currState;

    if (m_currState != State::DISABLED)
    {
        const SDL_FPoint mousePos = { mouse.viewPos.x, mouse.viewPos.y };

        if (Contains(mousePos))
        {
            if (mouse.leftDown)
            {
                m_currState = State::DOWN;

                auto r = GetCanvasRect();
                const float delta = mousePos.x - r.x;
                m_value = delta/r.w;

                m_scene.GetAssetManager().PlaySound(
                    SoundID::SYSTEM_SELECT, ChannelID::SYSTEM_1
                );
                
                // Evite le déclanchement de plusieurs boutons 
                mouse.leftDown = false;
            }
            else if (mouse.leftReleased)
            {
                if (m_listener)
                {
                    m_listener->OnValueChange(m_value);
                    // Evite le déclanchement de plusieurs boutons 
                    mouse.leftReleased = false;
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
        }
    }

    if (m_currState != m_prevState && m_listener)
    {
        // Callback "OnStateChanged"
        m_listener->OnStateChange(m_currState);

        if (m_currState == State::HOVER)
        {
            m_scene.GetAssetManager().PlaySound(
                SoundID::SYSTEM_HOVER, ChannelID::SYSTEM_2
            );
        }
    }

    // Met à jour les textes du slider
    for (int i = 0; i < 4; i++)
    {
        if (m_texts[i] == nullptr) continue;

        const bool enabled = (i == int(m_currState));
        if (m_texts[i]->IsEnabled() != enabled)
        {
            m_texts[i]->SetEnabled(enabled);
        }
    }
}

void Slider::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    SDL_Texture *texture = nullptr;

    if (m_atlasPart)
    {
        texture = m_atlasPart->GetTexture();

        const SDL_Rect *srcRect = nullptr;

        switch (m_currState)
        {
        case State::UP:
            srcRect = m_atlasPart->GetSrcRect(0);
            break;
        case State::HOVER:
            srcRect = m_atlasPart->GetSrcRect(2);
            break;
        case State::DOWN:
            srcRect = m_atlasPart->GetSrcRect(2);
            break;
        case State::DISABLED:
        default:
            srcRect = m_atlasPart->GetSrcRect(3);
            break;
        }
        
        const SDL_FRect dstRect = GetCanvasRect();
    
        //Render the bg.
        const SDL_FRect fillerDst = {dstRect.x, dstRect.y, m_value * dstRect.w, dstRect.h};
        const SDL_FRect edge1Dst = {dstRect.x, dstRect.y, dstRect.h, dstRect.h};
        const SDL_FRect edge2Dst = {dstRect.x + (fillerDst.w <= dstRect.h ? 0 : fillerDst.w - dstRect.h), dstRect.y, dstRect.h, dstRect.h};
        
        
        RE_RenderCopyF(renderer, texture, m_atlasPart->GetSrcRect(1), &fillerDst, RE_Anchor::NORTH_WEST);
        RE_RenderCopyF(renderer, texture, m_atlasPart->GetSrcRect(1), &edge1Dst, RE_Anchor::NORTH_WEST);
        RE_RenderCopyF(renderer, texture, m_atlasPart->GetSrcRect(1), &edge2Dst, RE_Anchor::NORTH_WEST);

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

void Slider::SetText(Text *text, State state)
{
    if (m_texts[int(state)])
    {
        m_texts[int(state)]->Delete();
    }

    m_texts[int(state)] = text;
    if (text)
    {
        text->SetParent(this);
        text->SetEnabled(state == m_currState);
    }
}

void Slider::SetValue(float v)
{
    m_value = v;
    if (m_listener)
    {
        m_listener->OnValueChange(v);
    }
}

