#include "Firefly.h"


Firefly::Firefly(Scene &scene, bool respawn)
    : Collectable(scene, Layer::COLLECTABLE)
{
    m_name = "Firefly";
    m_respawn = respawn;
    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::COLLECTABLE);
    AssertNew(atlas);

    RE_AtlasPart *part = atlas->GetPart("Firefly");
    AssertNew(part);
    RE_TexAnim *ffAnim = new RE_TexAnim(m_animator, "Firefly", part);
    ffAnim->SetCycleCount(-1);
    ffAnim->SetCycleTime(1);
}

Firefly::~Firefly()
{
}



void Firefly::Start() {

    m_animator.PlayAnimation("Firefly");
    PE_World& world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::STATIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = "Firefly";
    bodyDef.damping.SetZero();
    PE_Body* body = world.CreateBody(bodyDef);
    SetBody(body);

    // Crée le collider
    PE_CircleShape circle(PE_Vec2(0.0f, 0.45f), 0.45f);
    PE_ColliderDef colliderDef;
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = CATEGORY_COLLECTABLE;
    colliderDef.filter.maskBits = CATEGORY_ENEMY | CATEGORY_PLAYER | CATEGORY_TERRAIN;
    colliderDef.shape = &circle;
    PE_Collider* collider = body->CreateCollider(colliderDef);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simulés
    body->SetAwake(false);
}

void Firefly::Render() {
    SDL_Renderer* renderer = m_scene.GetRenderer();
    Camera* camera = m_scene.GetActiveCamera();

    m_animator.Update(m_scene.GetTime());

    float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0 };
    rect.h = 1.0f * scale;
    rect.w = 1.0f * scale;
    camera->WorldToView(GetPosition(), rect.x, rect.y);
    m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
}

void Firefly::OnRespawn() {
   
    if (!m_respawn) {
        SetToRespawn(false);
        SetBodyEnabled(false);
        SetEnabled(false);
    }
    else {
        SetToRespawn(true);
        SetBodyEnabled(true);
        SetEnabled(true);
    }
    PE_Body* body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Firefly");
}

void Firefly::Collect(GameBody *collector) {
    if (Player* player = dynamic_cast<Player*>(collector)) {
        player->AddFirefly(1);//une luciole =1 point
        SetEnabled(false);
    }
}