#include "Nut.h"
#include "Scene.h"
#include "Camera.h"
#include "LevelScene.h"
#include "Graphics.h"
#include "Bulletlaunch.h"

Nut::Nut(Scene &scene)
	: Enemy(scene)
	, m_animator()
{
    m_name = "Nut";

    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::ENEMY);
    AssertNew(atlas);

    // Animation "Idle"
    RE_AtlasPart *part = atlas->GetPart("NutIdle");
    AssertNew(part);
    RE_TexAnim *idleAnim = new RE_TexAnim(m_animator, "Idle", part);
    idleAnim->SetCycleCount(0);
    
    part = atlas->GetPart("NutSpinning");
    AssertNew(part);
    RE_TexAnim *spinningAnim = new RE_TexAnim(m_animator, "Spinning", part);
    spinningAnim->SetCycleCount(-1);
    spinningAnim->SetCycleTime(1);
    
    part = atlas->GetPart("NutDying");
    AssertNew(part);
    RE_TexAnim *dyingAnim = new RE_TexAnim(m_animator, "Dying", part);
    dyingAnim->SetCycleCount(0);
}

Nut::~Nut()
{
}

void Nut::Start()
{
    SetToRespawn(true);

    // Joue l'animation par d�faut
    m_animator.PlayAnimation("Idle");

    // Cr�e le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = "Nut";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    
    // Cr�e le collider
    PE_CircleShape circle(PE_Vec2(0.0f, 0.45f), 0.45f);
    PE_ColliderDef colliderDef;
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = CATEGORY_ENEMY;
    colliderDef.filter.maskBits = CATEGORY_COLLECTABLE | CATEGORY_ENEMY | CATEGORY_PLAYER | CATEGORY_TERRAIN;
    colliderDef.shape = &circle;
    PE_Collider *collider = body->CreateCollider(colliderDef);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simul�s
    body->SetAwake(false);
}

void Nut::FixedUpdate()
{
    PE_Body *body = GetBody();
    PE_Vec2 position = body->GetPosition();
    PE_Vec2 velocity = body->GetLocalVelocity();

    // Tue la noisette si elle tombe dans un trou
    if (position.y < -2.0f)
    {
        SetEnabled(false);
        SetToRespawn(true);
        return;
    }

    if (body->IsAwake() == false)
    {
        // Ne met pas � jour la noisette si elle est endormie
        // Le joueur est loin d'elle et elle n'est plus visible par la cam�ra.
        return;
    }

    LevelScene *levelScene = dynamic_cast<LevelScene *>(&m_scene);
    if (levelScene == nullptr)
    {
        assert(false);
        return;
    }

    Player *player = levelScene->GetPlayer();

    float dist = PE_Distance(position, player->GetPosition());

    if (dist > 24.0f)
    {
        // La distance entre de joueur et la noisette vient de d�passer 24 tuiles.
        // On endort la noisette pour ne plus la simuler dans le moteur physique.
        m_state = State::IDLE;
        body->SetAwake(false);
        return;
    }

    if (m_state == State::DYING)
    {
        m_animator.PlayAnimation("Dying");
        m_timer += m_scene.GetFixedTimeStep();
        if (m_timer >= 2.f)
        {
            SetEnabled(false);
        } else
        {
            body->SetVelocity({0.f, -5.f});
        }
        return;
    }

    PE_Vec2 mvt = (player->GetPosition().x - position.x) < 0
            ? PE_Vec2{-2.f, 0.f}
            : PE_Vec2{2.f, 0.f};
    ;
    body->SetVelocity(mvt);
    if (m_state == State::IDLE)
    {
        m_state = State::SPINNING;
        m_animator.PlayAnimation("Spinning");
    }
    if (dist <= 5.0f && m_state != State::JUMPED)
    {
        body->ApplyImpulse({0.f, 50.f});
        m_state = State::JUMPED;
    }

    if (m_isBounced)
    {
        body->SetVelocity(body->GetVelocity() - m_bounce);
        m_isBounced = false;
    }
}

void Nut::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();

    m_animator.Update(m_scene.GetTime());

    float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0 };
    rect.h = 1.0f * scale;
    rect.w = 1.0f * scale;
    camera->WorldToView(GetPosition(), rect.x, rect.y);
    m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
}

void Nut::OnRespawn()
{
    m_state = State::IDLE;
    m_isBounced = false;
    m_timer = 0.f;

    SetToRespawn(true);
    SetBodyEnabled(true);
    SetEnabled(true);

    PE_Body *body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Idle");
}

void Nut::Damage(GameBody *damager)
{
	if (Player *player = dynamic_cast<Player *>(damager)) {
        player->Bounce();
	    m_state = State::DYING;
	}
    if (Bulletlaunch* bullet = dynamic_cast<Bulletlaunch*>(damager)) {
        m_state = State::DYING;
    }
}

void Nut::OnCollisionStay(GameCollision &collision)
{
    PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;
    PE_Body* body = GetBody();
    if (m_state == State::DYING)
    {
        collision.SetEnabled(false);
        
        return;
    }
    
        
    // Collision avec le joueur
    if (otherCollider->CheckCategory(CATEGORY_PLAYER))
    {
        Player *player = dynamic_cast<Player *>(collision.gameBody);
        if (player == nullptr)
        {
            assert(false);
            return;
        }
        const float angle = PE_SignedAngleDeg(manifold.normal, PE_Vec2::down);
        if (fabsf(angle) > PLAYER_DAMAGE_ANGLE)
        {
            player->Damage();
        }
        return;
    } else if (otherCollider->CheckCategory(CATEGORY_ENEMY))
    {
        if (Nut *nut = dynamic_cast<Nut *>(collision.gameBody))
        {
            nut->Bounce(manifold.normal * 40.f);
            Bounce(manifold.normal * 4.f);
        }
    }
}

void Nut::Bounce(const PE_Vec2 &v)
{
    if (m_isBounced)
    {
        m_bounce += v;
        m_bounce /= 2;
    }
    else
    {
        m_isBounced = true;
        m_bounce = v;
    }
}