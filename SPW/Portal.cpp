#include "Portal.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "Camera.h"
#include "LevelScene.h"

Portal::Portal(Scene& scene, int index) :
    Collectable(scene, Layer::COLLECTABLE)
{
    m_name = "Heart";
    m_index = index;
    
    // Animation "Base"
   
        RE_Atlas* atlas = scene.GetAssetManager().GetAtlas(AtlasID::PORTAL);
        AssertNew(atlas);
        RE_AtlasPart* part = atlas->GetPart("Green");
        AssertNew(part);
        RE_TexAnim* anim = new RE_TexAnim(m_animator, "vert", part);
        anim->SetCycleCount(0);
   
    
    
    part = atlas->GetPart("Red");
    AssertNew(part);
    RE_TexAnim* RunningAnim = new RE_TexAnim(m_animator, "rouge", part);
    RunningAnim->SetCycleCount(-1);
    RunningAnim->SetCycleTime(1);
}

void Portal::Start()
{

    SetToRespawn(true);
    if (m_index == 1) {
        m_animator.PlayAnimation("vert");
    }
    else {
        m_animator.PlayAnimation("rouge");
    }
    PE_World& world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::STATIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = "Portal";
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

void Portal::Render()
{
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

void Portal::OnRespawn()
{
    m_state = State::IDLE;
    
    SetToRespawn(true);
    SetBodyEnabled(true);
    SetEnabled(true);
    
    PE_Body* body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    if (m_index == 1) {
        m_animator.PlayAnimation("vert");
    }
    else {
        m_animator.PlayAnimation("rouge");
    }
}

Portal::~Portal()
{
}
void Portal::FixedUpdate()
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
}

void Portal::OnCollisionEnter(GameCollision& collision)
{
    LevelScene* lvlScn = dynamic_cast<LevelScene*>(&m_scene);
    if (lvlScn == nullptr)
        return;

    Player* player = lvlScn->GetPlayer();
    if (player == nullptr) {
        return;
    }

    if (m_index == 1) {
        if (collision.otherCollider->CheckCategory(CATEGORY_PLAYER))
        {
            player->positionportal = m_scene.portal2->GetPosition() + PE_Vec2{1.5f, 0.f};
            player->takeportal = true;
        }
    }
    else if (m_index == 2) {
        if (collision.otherCollider->CheckCategory(CATEGORY_PLAYER))
        {
            
            player->positionportal = m_scene.portal1->GetPosition() + PE_Vec2{1.5f, 0.f};
            player->takeportal = true;
        }
    }
}

void Portal::Collect(GameBody* collector)
{
}
