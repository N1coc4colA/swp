#include "Oneway.h"
#include "Scene.h"
#include "Camera.h"
#include "LevelScene.h"


Oneway::Oneway(Scene &scene, int size, PE_Vec2 pos, int sens)
	: Enemy(scene)
	, m_animator(),m_size(size),m_sens(sens), m_fixedCount(0)
{
    m_name = "Oneway";
    m_oneways = nullptr;
    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    AssertNew(atlas);
   
    // Animation "Idle"
    
   
    RE_AtlasPart* part = atlas->GetPart("OneWay");
    AssertNew(part);
    m_state = State::BODY;
    RE_TexAnim* idleAnim = new RE_TexAnim(m_animator, "Body", part);
    idleAnim->SetCycleCount(0);
    
    if (m_size > 1) {
        Oneway* m_oneways = new Oneway(scene, m_size-1,pos,m_sens);
        m_oneways->SetStartPosition(pos);
    }

    
}

Oneway::~Oneway()
{
}

void Oneway::Start()
{
    SetToRespawn(true);

    // Joue l'animation par défaut
    
        
    
    m_animator.PlayAnimation("Body");

    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::KINEMATIC;

    
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    m_posstart  = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    m_posmax = GetStartPosition() + PE_Vec2((m_sens*m_size)+.5f, 0.0f);


    bodyDef.name = "Oneway";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);
    m_retour = true;
    // Crée le collider
    PE_PolygonShape box(-0.5f, 0.f, 0.5f, 1.f);
    PE_ColliderDef colliderDef;
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.filter.maskBits = CATEGORY_ENEMY | CATEGORY_PLAYER | CATEGORY_TERRAIN;
    colliderDef.shape = &box;
    m_collider = body->CreateCollider(colliderDef);
    
    
}

void Oneway::FixedUpdate()
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

    if (m_fixedCount == 180)
    {
        m_retour = !m_retour;
        m_fixedCount = 0;
    }
   
    if (m_retour) {
        

		PE_Vec2 mvt(+m_sens * 1.f * m_size, 0.f);
		body->SetVelocity(mvt);
	}
	else {
		PE_Vec2 mvt(-m_sens * 1.f * m_size, 0.f);
		body->SetVelocity(mvt);
	}
    
}

void Oneway::Render()
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

void Oneway::OnRespawn()
{
    
    m_isBounced = false;

    SetToRespawn(true);
    SetBodyEnabled(true);
    SetEnabled(true);

    PE_Body *body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Body");
}





