#include "Bonus.h"
#include "Scene.h"
#include "Camera.h"
#include "Heart.h"

Bonus::Bonus(Scene &scene) :
    GameBody(scene, Layer::TERRAIN_FOREGROUND), m_animator(), m_hit(false)
{
    m_name = "BonusFull";
    RE_Atlas* atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    AssertNew(atlas)
        RE_AtlasPart* part = atlas->GetPart("BonusFull");
    AssertNew(part)
        RE_TexAnim* anim = new RE_TexAnim(m_animator, "Base", part);
    anim->SetCycleCount(0);

}

void Bonus::Start()
{
    SetToRespawn(true);

    // Joue l'animation par défaut
    m_animator.PlayAnimation("Base");

    // Crée le corps
    PE_World& world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::STATIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = "BonusFull";
    bodyDef.damping.SetZero();
    PE_Body* body = world.CreateBody(bodyDef);
    SetBody(body);

    // Crée le collider
    PE_PolygonShape box( -0.5f, 0.0f, 0.5f,1.f);
    PE_ColliderDef colliderDef;
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.filter.maskBits = CATEGORY_PLAYER;
    colliderDef.shape = &box;
    PE_Collider* collider = body->CreateCollider(colliderDef);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simulés
    body->SetAwake(false);

}

void Bonus::Render()
{
    if (m_active)
    {
        SDL_Renderer* renderer = m_scene.GetRenderer();
        const Camera* camera = m_scene.GetActiveCamera();

        m_animator.Update(m_scene.GetTime());

        const float scale = camera->GetWorldToViewScale();
        SDL_FRect rect = { 0 };
        rect.w = scale;
        rect.h = scale;
        camera->WorldToView(GetPosition(), rect.x, rect.y);
        m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
    }
}

void Bonus::OnRespawn()
{
}

void Bonus::OnCollisionEnter(GameCollision &collision)
{
    
}
