#include "Checkpoint.h"
#include "Scene.h"
#include "Camera.h"
#include "LevelScene.h"

Checkpoint::Checkpoint(Scene &scene) :
    GameBody(scene, Layer::TERRAIN_BACKGROUND), m_animator(), m_isActive(false)
{
    m_name = "Checkpoint";
  

    // Animation "Base"
    RE_Atlas* atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    AssertNew(atlas);

    RE_AtlasPart* part = atlas->GetPart("CheckpointFull");
    AssertNew(part);
    RE_TexAnim* anim = new RE_TexAnim(m_animator, "Full", part);
    anim->SetCycleCount(0);
    part = atlas->GetPart("CheckpointEmpty");
    AssertNew(part);
    RE_TexAnim* Emptyanim = new RE_TexAnim(m_animator, "Empty", part);
    anim->SetCycleCount(0);
}

void Checkpoint::Start()
{
    // Joue l'animation par défaut
    m_animator.PlayAnimation("Empty");
    
   

    // Crée le corps
    PE_World& world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::STATIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = "LevelCheckpoint";
    PE_Body* body = world.CreateBody(bodyDef);
    SetBody(body);

    // Crée le collider
    PE_PolygonShape box(-0.4f, 0.0f, 0.4f, 1.9f);
    PE_ColliderDef colliderDef;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.shape = &box;
    colliderDef.isTrigger = true;
    PE_Collider* collider = body->CreateCollider(colliderDef);
}

void Checkpoint::Render()
{
    SDL_Renderer* renderer = m_scene.GetRenderer();
    Camera* camera = m_scene.GetActiveCamera();
    if (empty) {
        m_animator.PlayAnimation("Empty");
    }
    else {
        m_animator.PlayAnimation("Full");
    }
    m_animator.Update(m_scene.GetTime());

    float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0 };
    rect.w = 1.0f * scale;
    rect.h = 2.0f * scale;
    camera->WorldToView(GetPosition(), rect.x, rect.y);
    m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
}

void Checkpoint::OnCollisionEnter(GameCollision &collision)
{
    LevelScene* lvlScn = dynamic_cast<LevelScene*>(&m_scene);
    if (lvlScn == nullptr)
        return;
    
    Player* player = lvlScn->GetPlayer();
    if (collision.otherCollider->CheckCategory(CATEGORY_PLAYER))
    {
        PE_Vec2 position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
        player->SetStartPosition(position); 
        empty = true;
    }
}
