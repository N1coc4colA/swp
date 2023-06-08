#include "Bulletlaunch.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "Camera.h"
#include "LevelScene.h"
#include "Enemy.h"
#include "Snake.h"
#include "Boss.h"

Bulletlaunch::Bulletlaunch(Scene &scene, bool Enemie,PE_Vec2 senstir) :
    Collectable(scene, Layer::COLLECTABLE)
{
    m_name = "Bulletlaunch";
    m_senstir = senstir;
    m_enemie = Enemie;
    // Animation "Base"
    RE_Atlas* atlas = scene.GetAssetManager().GetAtlas(AtlasID::BOSS);
    AssertNew(atlas);
    RE_AtlasPart* part = atlas->GetPart("BossBullet");
    AssertNew(part);
    RE_TexAnim* anim = new RE_TexAnim(m_animator, "IDLE",part);
    anim->SetCycleCount(0);

    atlas = scene.GetAssetManager().GetAtlas(AtlasID::BOSS);
    AssertNew(atlas);
    part = atlas->GetPart("BossBullet");
    AssertNew(part);
    RE_TexAnim* RunningAnim = new RE_TexAnim(m_animator, "RUNNING", part);
    RunningAnim->SetCycleCount(-1);
    RunningAnim->SetCycleTime(1);
}

void Bulletlaunch::Start()
{
    SetToRespawn(false);
    m_animator.PlayAnimation("IDLE");
    PE_World& world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = "Bullet";
    bodyDef.damping.SetZero();

    PE_Body* body = world.CreateBody(bodyDef);
    SetBody(body);
    body->SetGravityScale(0.f);

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

void Bulletlaunch::Render()
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

void Bulletlaunch::OnRespawn()
{
    m_state = State::IDLE;

    SetToRespawn(false);
    SetBodyEnabled(true);
    SetEnabled(true);

    PE_Body* body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.5f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();

    m_animator.StopAnimations();
    m_animator.PlayAnimation("IDLE");
}

Bulletlaunch::~Bulletlaunch()
{
}
void Bulletlaunch::FixedUpdate()
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
        SetToRespawn(false);
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
    if (m_enemie) {
        
        body->SetVelocity(m_senstir);
    }
    else{
        body->SetVelocity(m_senstir);
    }
    
    
  

}

void Bulletlaunch::OnCollisionEnter(GameCollision &collision)
{
    const PE_Manifold& manifold = collision.manifold;
    PE_Collider* otherCollider = collision.otherCollider;
    if (otherCollider->CheckCategory(CATEGORY_TERRAIN)) {
        SetEnabled(false);
    }
    if (otherCollider->CheckCategory(CATEGORY_ENEMY)){
        
        if (!m_enemie) {
            if (Snake* snake = dynamic_cast<Snake*>(collision.gameBody)) {

                if (snake == nullptr)
                {
                    assert(false);
                    return;
                }

                SetEnabled(false);
                return;
            }
            if (Boss* boss = dynamic_cast<Boss*>(collision.gameBody)) {

                if (boss == nullptr)
                {
                    
                    assert(false);
                    return;
                }
                boss->Damage(this);
                SetEnabled(false);
                return;
            }
            Enemy* enemy = dynamic_cast<Enemy*>(collision.gameBody);
            if (enemy == nullptr)
            {
                assert(false);
                return;
            }
         
            printf("dameke\n");
            enemy->Damage(this);
            SetEnabled(false);
        }
        else {
           
        }
    }
}

void Bulletlaunch::Collect(GameBody *collector)
{
}
