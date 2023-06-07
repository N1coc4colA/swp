#include "KeyButton.h"

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
                if (false) //m_listener)
                {
                    // Exécute l'action associée au bouton
                    //m_listener->OnPress();

                    m_scene.GetAssetManager().PlaySound(
                        SoundID::SYSTEM_SELECT, ChannelID::SYSTEM_1
                    );

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
    /*for (int i = 0; i < 4; i++)
    {
        if (m_texts[i] == nullptr) continue;

        bool enabled = (i == int(m_currState));
        if (m_texts[i]->IsEnabled() != enabled)
        {
            m_texts[i]->SetEnabled(enabled);
        }
    }*/
}

void KeyButton::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    SDL_Texture *texture = NULL;

    if (m_atlasPart)
    {
        texture = m_atlasPart->GetTexture();

        SDL_FRect dstRect = GetCanvasRect();
        const SDL_Rect *srcRect = NULL;

        switch (m_currState)
        {
        case State::UP:
            srcRect = m_atlasPart->GetSrcRect(0);
            break;
        case State::HOVER:
            srcRect = m_atlasPart->GetSrcRect(1);
            break;
        case State::DOWN:
            srcRect = m_atlasPart->GetSrcRect(2);
            break;
        case State::DISABLED:
        default:
            srcRect = m_atlasPart->GetSrcRect(3);
            break;
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
