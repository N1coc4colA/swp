#include "Snake.h"
#include "Scene.h"
#include "Camera.h"
#include "LevelScene.h"
#include "Graphics.h"

Snake::Snake(Scene &scene, int size, PE_Vec2 pos, int sens, Layer layer)
	: Enemy(scene)
	, m_animator(),m_size(size),m_sens(sens), m_fixedCount(0)
{
    if (m_size == 0) {
        SetLayer(Layer::ENEMYHOUSE);
    } else if(m_size ==3) {
        SetLayer(Layer::ENEMYHEAD);
    }else{
        SetLayer(Layer::ENEMYBODY);
    }

    m_name = "Snake";
    m_snakes = nullptr;
    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::ENEMY);
    AssertNew(atlas);
    
    // Animation "Idle"
    if (m_size == 0) {
        
        RE_AtlasPart* part = atlas->GetPart("SerpentHouse");
        AssertNew(part);
        m_state = State::HOUSE;
        RE_TexAnim* idleAnim = new RE_TexAnim(m_animator, "House", part);
        idleAnim->SetCycleCount(0);
    }
    else if(m_size ==3){
        
        RE_AtlasPart* part = atlas->GetPart("SerpentHead");
        AssertNew(part);
        m_state = State::HEAD;
        RE_TexAnim* idleAnim = new RE_TexAnim(m_animator, "Head", part);
        idleAnim->SetCycleCount(0);
    }
    else {
        
        RE_AtlasPart* part = atlas->GetPart("SerpentBody");
        AssertNew(part);
        m_state = State::BODY;
        RE_TexAnim* idleAnim = new RE_TexAnim(m_animator, "Body", part);
        idleAnim->SetCycleCount(0);
    }
    if (m_size >= 1) {
        Snake* m_snakes = new Snake(scene, m_size-1,pos,m_sens, Layer::ENEMYHOUSE);
        m_snakes->SetStartPosition(pos);
    }

    
}

Snake::~Snake()
{
}

void Snake::Start()
{
    SetToRespawn(true);

    // Joue l'animation par défaut
    
        
    
    if (m_state == State::HOUSE) {
        m_animator.PlayAnimation("House");
    }
    else if(m_state == State::HEAD) {
        m_animator.PlayAnimation("Head");
    }
    else {
        m_animator.PlayAnimation("Body");
    }

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
    colliderDef.filter.maskBits = CATEGORY_COLLECTABLE |CATEGORY_ENEMY | CATEGORY_PLAYER | CATEGORY_TERRAIN;
    colliderDef.shape = &box;
    m_collider = body->CreateCollider(colliderDef);
    
    
}

void Snake::FixedUpdate()
{
    PE_Body* body = GetBody();
    if (body == nullptr)
        return;

    PE_Vec2 position = body->GetPosition();
    PE_Vec2 velocity = body->GetLocalVelocity();

    m_fixedCount++;



    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();
    
    PE_Vec2 positionstart = body->GetPosition();

    if (m_fixedCount == 119)
    {
        m_retour = !m_retour;
        m_fixedCount = 0;
    }
   
    if (m_retour) {
        

		PE_Vec2 mvt(+m_sens * 0.5f * m_size, 0.f);
		body->SetVelocity(mvt);
	}
	else {
		PE_Vec2 mvt(-m_sens * 0.5f * m_size, 0.f);
		body->SetVelocity(mvt);
	}

}

void Snake::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();
    SDL_RendererFlip flip = m_sens == 1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    m_animator.Update(m_scene.GetTime());

    float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0 };
    rect.h = 1.0f * scale;
    rect.w = 1.0f * scale;
    camera->WorldToView(GetPosition(), rect.x, rect.y);
    m_animator.RenderCopyExF(&rect, RE_Anchor::SOUTH,0.,{0.f,0.f}, flip);
}

void Snake::OnRespawn()
{
    if (m_size == 0) {
        m_state = State::HOUSE;
    }
    else if (m_size == 3) {
        
        m_state = State::HEAD;
    }
    else {
        m_state = State::BODY;
    }
    m_state = State::HEAD;
    m_isBounced = false;

    SetToRespawn(true);
    SetBodyEnabled(true);
    SetEnabled(true);

    PE_Body* body = GetBody();
    //body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    if (m_size == 0) {
        m_animator.PlayAnimation("House");
    }
    else if (m_size == 3) {

        m_animator.PlayAnimation("Head");
    }
    else {
        m_animator.PlayAnimation("Body");
    }
    
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

