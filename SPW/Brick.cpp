#include "Brick.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"


Brick::Brick(Scene &scene)
    : GameBody(scene, Layer::TERRAIN)
    , m_animator()
{
    m_name = "Brick";
    
    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    AssertNew(atlas)
    RE_AtlasPart *part = atlas->GetPart("Brick");
    AssertNew(part)
    RE_TexAnim *anim = new RE_TexAnim(m_animator, "Base", part);
    anim->SetCycleCount(0);
}

void Brick::Start()
{
    SetToRespawn(true);
    m_animator.PlayAnimation("Base");

    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition()+ PE_Vec2(0.5f, 0.0f);;
    bodyDef.name = "Brick";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    // Crée le collider
    PE_PolygonShape box(-0.5f, 0.f, 0.5f, 1.f);
    PE_ColliderDef colliderDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.filter.maskBits = CATEGORY_PLAYER;
    colliderDef.shape = &box;
    PE_Collider *collider = body->CreateCollider(colliderDef);
}

void Brick::FixedUpdate()
{
    PE_Body *body = GetBody();
    if (m_active)
    {
        body->SetGravityScale(0.f);
        body->SetVelocity({0.f, (GetStartPosition() - body->GetPosition()).y});
    } else {
        body->SetGravityScale(1.f);
        if (body->GetPosition().y >= 0.f)
        {
            body->SetVelocity({0.f, -5.f});
        } else
        {
            SetEnabled(false);
        }
    }
}


void Brick::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    const Camera *camera = m_scene.GetActiveCamera();

    m_animator.Update(m_scene.GetTime());

    const float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0, 0, scale, scale};
    camera->WorldToView(GetPosition(), rect.x, rect.y);
    m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
}

void Brick::touchedFromBottom()
{
    m_active = false;
}

void Brick::OnRespawn()
{
    m_active = true;
    
    SetToRespawn(true);
    SetBodyEnabled(true);
    SetEnabled(true);

    PE_Body *body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Base");
}

void Brick::OnCollisionStay(GameCollision &collision)
{
    if (!m_active)
    {
        collision.SetEnabled(false);
        collision.ResolveUp();
        return;
    }
    if (collision.otherCollider->CheckCategory(CATEGORY_PLAYER))
    {
        collision.ResolveUp();
        collision.SetEnabled(false);
    }
}
