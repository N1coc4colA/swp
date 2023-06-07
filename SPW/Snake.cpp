#include "Snake.h"
#include "Scene.h"
#include "Camera.h"
#include "LevelScene.h"
#include "Graphics.h"

Snake::Snake(Scene &scene, int size, PE_Vec2 pos, int sens)
	: Enemy(scene)
	, m_animator(),m_size(size),m_sens(sens)
{
    m_name = "Snake";

    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::UI);
    AssertNew(atlas);
   
    // Animation "Idle"
    RE_AtlasPart* part = atlas->GetPart("House");
    AssertNew(part);
    RE_TexAnim* idleAnim = new RE_TexAnim(m_animator, "Idle", part);
    idleAnim->SetCycleCount(0);
    if (m_size >= 1) {
        Snake* snakes = new Snake(scene, m_size-1,pos,m_sens);
        snakes->SetStartPosition(pos);
    }

    
}

Snake::~Snake()
{
}

void Snake::Start()
{
    SetToRespawn(true);

    // Joue l'animation par défaut
    m_animator.PlayAnimation("Idle");

    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::KINEMATIC;

    
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    m_posstart  = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    m_posmax = GetStartPosition() + PE_Vec2((m_sens*m_size)+.5f, 0.0f);


    bodyDef.name = "Snake";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);
    m_retour = true;
    // Crée le collider
    PE_PolygonShape box(-0.5f, 0.f, 0.5f, 1.f);
    PE_ColliderDef colliderDef;
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = CATEGORY_ENEMY;
    colliderDef.filter.maskBits = CATEGORY_ENEMY | CATEGORY_PLAYER | CATEGORY_TERRAIN;
    colliderDef.shape = &box;
    m_collider = body->CreateCollider(colliderDef);
    
    
    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simulés
    body->SetAwake(false);
}

void Snake::FixedUpdate()
{
    PE_Body* body = GetBody();
    if (body == nullptr)
        return;

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
        // Ne met pas à jour la noisette si elle est endormie
        // Le joueur est loin d'elle et elle n'est plus visible par la caméra.
        return;
    }

    LevelScene* levelScene = dynamic_cast<LevelScene*>(&m_scene);
    if (levelScene == nullptr)
    {
        assert(false);
        return;
    }

    Player* player = levelScene->GetPlayer();

    float dist = PE_Distance(position, player->GetPosition());

    if (dist > 24.0f)
    {
        // La distance entre de joueur et la noisette vient de dépasser 24 tuiles.
        // On endort la noisette pour ne plus la simuler dans le moteur physique.
        body->SetAwake(false);
        return;
    }

    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();
    
    PE_Vec2 positionstart = body->GetPosition();
   
    if (m_retour) {
        PE_Vec2 mvt = PE_Vec2{ m_sens*0.5f*m_size, 0.f };
        body->SetVelocity(mvt);
        if (m_sens == 1) {
            if (position.x >= m_posmax.x) {
                m_retour = false;
            }
        }
        else {
            if (position.x <= m_posmax.x) {
                m_retour = false;
            }
        }
    }
    else {
        PE_Vec2 mvt = PE_Vec2{ -1*m_sens*0.5f*m_size, 0.f };
        body->SetVelocity(mvt); 
        if (m_sens == 1) {
            if (position.x <= m_posstart.x) {
                m_retour = true;
            }
        }
        else {
            if (position.x >= m_posstart.x) {
                m_retour = true;
            }
        }
    }
    if (m_state == State::IDLE)
    {
        m_state = State::DASH;
        m_animator.PlayAnimation("Idle");
    }


}

void Snake::Render()
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

void Snake::OnRespawn()
{
    m_state = State::IDLE;
    m_isBounced = false;

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

void Snake::Damage(GameBody *damager)
{
	if (Player *player = dynamic_cast<Player *>(damager)) {
        player->Damage();
        
	}
}

void Snake::OnCollisionStay(GameCollision &collision)
{
    PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;

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
    }
}

