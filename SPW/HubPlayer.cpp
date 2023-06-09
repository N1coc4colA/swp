#include "HubPlayer.h"
#include "Camera.h"
#include "Scene.h"
#include "Collectable.h"
#include "Enemy.h"
#include "Graphics.h"
#include  "Brick.h"
#include  "Bonus.h"
#include "Shield.h"
#include "Bullet.h"
#include "Bulletlaunch.h"
#include "LevelBlock.h"

HubPlayer::HubPlayer(Scene &scene)
	: Player(scene)
{
    m_name = "HubPlayer";
}

void HubPlayer::Start()
{
    // Joue l'animation par défaut
    m_animator.PlayAnimation("Idle");

    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.5f);
    bodyDef.name = "HubPlayer";
    bodyDef.damping.SetZero();
	bodyDef.mass = 4.f;
    bodyDef.gravityScale = 0.f;
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    // Création du collider
    PE_ColliderDef colliderDef;

    PE_CapsuleShape capsule(PE_Vec2(0.0f, 0.35f), PE_Vec2(0.0f, 0.85f), 0.35f);
    colliderDef.friction = 2.f;
    colliderDef.filter.categoryBits = CATEGORY_PLAYER;
    //colliderDef.filter.maskBits = CATEGORY_TERRAIN;
    colliderDef.shape = &capsule;
    PE_Collider *collider = body->CreateCollider(colliderDef);
}

void HubPlayer::Update()
{
    const ControlsInput &controls = m_scene.GetInputManager().GetControls();

    // Sauvegarde les contrôles du joueur pour modifier
    // sa physique au prochain FixedUpdate()

    m_direction = Direction::NONE;
    
    m_hDirection = controls.hAxis;
    
    if (controls.vAxis < 0.f)
    {
        m_direction = Direction::DOWNWARD;
        //m_goingDown = true;
    } else if (controls.vAxis > 0.f)
    {
        m_direction = Direction::UPWARD;
        //m_jump = true;
    }
    else if (m_hDirection < 0.f)
    {
        m_direction = Direction::BACKWARD;
    } else if (m_hDirection > 0.f)
    {
        m_direction = Direction::FORWARD;
    }
}

void HubPlayer::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();

    // Met à jour les animations du joueur
    m_animator.Update(m_scene.GetTime());
    if (timer_start)
    {
        m_shieldAanimator.Update(m_scene.GetTime());
    }

    float scale = camera->GetWorldToViewScale();
    SDL_RendererFlip flip = m_state == State::DYING ? SDL_FLIP_VERTICAL : (m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    if (m_state == State::DYING) {
        m_state = State::DEAD;
        m_scene.Quit();
   }

    SDL_FRect rect = { 0, 0, 1.375f * scale, rect.w = 1.f * scale };
    PE_Vec2 pos = GetPosition();
	//pos.y += 0.1f;
    camera->WorldToView(pos, rect.x, rect.y);
    
    // Dessine l'animation du joueur
    m_animator.RenderCopyExF(&rect, RE_Anchor::SOUTH, 0., Vec2(0.5f, 0.5f), flip);
    if (timer_start)
    {
        m_shieldAanimator.RenderCopyExF(&rect, RE_Anchor::SOUTH, 0., Vec2(0.5f, 0.5f), SDL_FLIP_NONE);
    }
}

void HubPlayer::FixedUpdate()
{
    PE_Body *body = GetBody();
    const PE_Vec2 position = body->GetPosition();
    PE_Vec2 velocity = body->GetLocalVelocity();

    // Réveille les corps autour du joueur
    WakeUpSurroundings();

    if (m_state == State::DEAD) {
        Kill();
        return;
    }
    
    //--------------------------------------------------------------------------
    // Etat du joueur

    Player::State tmpState = m_state;

    PE_Vec2 move = PE_Vec2::zero;

    switch (m_direction)
    {
    case Direction::FORWARD:
        {
            body->SetPosition(GetPosition() + PE_Vec2{0.05f, 0.f});
            tmpState = State::RUNNING;
            break;
        }
    case Direction::BACKWARD:
        {
            body->SetPosition(GetPosition() + PE_Vec2{-0.05f, 0.f});
            tmpState = State::RUNNING;
            break;
        }
    case Direction::UPWARD:
        {
            body->SetPosition(GetPosition() + PE_Vec2{0.f, 0.05f});
            tmpState = State::SKIDDING;
            break;
        }
    case Direction::DOWNWARD:
        {
            body->SetPosition(GetPosition() + PE_Vec2{0.f, -0.05f});
            tmpState = State::FALLING;
        }
    case Direction::NONE:
        {
            tmpState = State::IDLE;
        }
    }

    RayHit hit = m_scene.RayCast(GetPosition(), move.Normalize(), 1.f, CATEGORY_TERRAIN, false);

    if (hit.collider)
    {
        body->SetPosition(GetPosition() + move);
    }
    
    if (m_state != tmpState)
    {
        switch (tmpState) {
        case State::SKIDDING: {
                m_animator.PlayAnimation("Skidding");
                break;
        }
        case State::FALLING: {
                m_animator.PlayAnimation("Falling");
                break;
        }
        case State::IDLE: {
                m_animator.PlayAnimation("Idle");
                break;
        }
        case State::RUNNING: {
                m_animator.PlayAnimation("Running");
                break;
        }
        case State::DYING: {
                m_animator.PlayAnimation("Dying");
                break;
        }
        default: {
                break;
        }
        }
        m_state = tmpState;
    }
    
    // Orientation du joueur
    const bool oldFacing = m_facingRight;
    if (m_hDirection < 0.f)
    {
        m_facingRight = false;
    } else if (m_hDirection > 0.f)
    {
        m_facingRight = true;
    }
}

void HubPlayer::OnRespawn()
{
    PE_Body *body = GetBody();
    AssertNew(body)

    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);

   
    m_state = State::IDLE;
    m_hDirection = 0.0f;
    m_facingRight = true;
    m_bounce = false;
    m_jump = false;
    m_jumpedOnce = false;

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Idle");

    SetToRespawn(true);
}

void HubPlayer::DrawGizmos()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Graphics graphics(renderer, *m_scene.GetActiveCamera());
    const PE_Vec2 position = GetPosition();
    const PE_Vec2 velocity = GetVelocity();

    // Dessine en blanc le vecteur vitesse du joueur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    graphics.DrawVector(0.5f * velocity, position);

    // Dessine en jaune les rayons pour la détection du sol
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    const PE_Vec2 originL = position + PE_Vec2(-0.35f, 0.0f);
    const PE_Vec2 originR = position + PE_Vec2(+0.35f, 0.0f);
    graphics.DrawVector(0.1f * PE_Vec2::down, originL);
    graphics.DrawVector(0.1f * PE_Vec2::down, originR);
    graphics.DrawVector(PE_Vec2::left * 0.8f, position);
    graphics.DrawVector(PE_Vec2::right * 0.8f, position);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    graphics.DrawVector(PE_Vec2::up * 1.5f, position);
}

void HubPlayer::OnCollisionEnter(GameCollision &collision)
{
}

void HubPlayer::OnCollisionStay(GameCollision &collision)
{
    if (m_scene.shutingDown)
    {
        collision.ResolveUp();
        collision.SetEnabled(false);
        SetEnabled(false);
        return;
    }

    if (collision.otherCollider->CheckCategory(CATEGORY_TERRAIN))
    {
        if (LevelBlock *lb = dynamic_cast<LevelBlock *>(collision.gameBody))
        {
            lb->trigger();
        }
        collision.SetEnabled(false);
    }
}

class WakeUpCallback : public PE_QueryCallback
{
public:
    WakeUpCallback() = default;
    
    inline bool ReportCollider(PE_Collider *collider) override
    {
        collider->GetBody()->SetAwake(true);
        return true;
    }
};
