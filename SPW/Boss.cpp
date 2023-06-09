#include "Boss.h"
#include "Scene.h"
#include "Camera.h"
#include "LevelScene.h"
#include "Graphics.h"
#include "Bulletlaunch.h"
#include "Bullet.h"
#include <cstdlib>
#include <random>


Boss::Boss(Scene &scene)
	: Enemy(scene)
	, m_animator()
{
    m_name = "Boss";

    
    RE_Atlas* atlas = scene.GetAssetManager().GetAtlas(AtlasID::BOSS);
    AssertNew(atlas);

    // Animation "Idle"
    RE_AtlasPart* part = atlas->GetPart("BossIdle");
    AssertNew(part);
    RE_TexAnim* idleAnim = new RE_TexAnim(m_animator, "Idle", part);
    idleAnim->SetCycleCount(0);

    atlas = scene.GetAssetManager().GetAtlas(AtlasID::BOSS);
    AssertNew(atlas);
    part = atlas->GetPart("BossShield");
    AssertNew(part);
    RE_TexAnim* shieldAnim = new RE_TexAnim(m_animator, "shield", part);
    shieldAnim->SetCycleCount(0);

}

Boss::~Boss()
{
}

void Boss::Start()
{
    SetToRespawn(true);

    // Joue l'animation par défaut
    m_animator.PlayAnimation("Idle");

    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f,0.f);
    bodyDef.name = "Boss";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    // Crée le collider
    PE_CircleShape circle(PE_Vec2(0.0f, 0.45f), 1.f);
    PE_ColliderDef colliderDef;
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = CATEGORY_ENEMY;
    colliderDef.filter.maskBits = CATEGORY_COLLECTABLE |CATEGORY_ENEMY | CATEGORY_PLAYER | CATEGORY_TERRAIN;
    colliderDef.shape = &circle;
    PE_Collider *collider = body->CreateCollider(colliderDef);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simulés
    body->SetAwake(false);
}

void Boss::FixedUpdate()
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
        // Ne met pas à jour la noisette si elle est endormie
        // Le joueur est loin d'elle et elle n'est plus visible par la caméra.
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
        // La distance entre de joueur et la noisette vient de dépasser 24 tuiles.
        // On endort la noisette pour ne plus la simuler dans le moteur physique.
        SetCloser(false);
        m_state = State::IDLE;
        body->SetAwake(false);
        
        return;
    }
    if (dist <= 15.0f )
    {
        SetCloser(true);
    }
    else {
        SetCloser(false);
    }
    if (m_shield) {
        m_animator.PlayAnimation("shield");
    }
    else {
        m_animator.PlayAnimation("Idle");
    }
    
    PE_Vec2 mvt = { 0.f,0.f };
    if ((player->GetPosition().x - position.x) < 0) {
        mvt= PE_Vec2{ -5.f, 0.f };
    }
    else {
        mvt = PE_Vec2{ 5.f, 0.f };
    }
    
    
    
    PE_Vec2 moove = {0.f,0.f};
    m_timermoove++;
    if (m_timermoove == 200) {
        if ((player->GetPosition().x - position.x) < 0) {
            moove = PE_Vec2{ -250.f, 0.f };
        }
        else {
            moove = PE_Vec2{ 250.f, 0.f };
        }
        m_timermoove = 0;
    }
    body->SetVelocity(mvt+moove);
   


    m_timer_bigshoot++;
    m_timer_shoot++;
    m_timer_shield++;
    if (m_timer_shoot == 150)
    {
        PE_Vec2 mvt;
        
        if ((player->GetPosition().x - position.x) > 0) {
            mvt = PE_Vec2{ 2.3f, 0.f };
            //positionbullet += {2.f, 3.f};
        }
        else {
            mvt = PE_Vec2{ -2.3f, 0.f };
            //positionbullet -= {2.f, -3.f};
        }
        
        Bulletlaunch* bullet = new Bulletlaunch(m_scene,true,mvt);
        bullet->SetStartPosition(position+mvt);
        m_timer_shoot = 0;
    }

    if (m_timer_bigshoot == 500) {
        PE_Vec2 mvt = PE_Vec2{ 2.3f, 0.f };
        Bulletlaunch* bullet = new Bulletlaunch(m_scene, true, mvt);
        bullet->SetStartPosition(position + mvt);
        mvt = PE_Vec2{ 2.3f, 0.f };
        Bulletlaunch* bullet2 = new Bulletlaunch(m_scene, true, mvt);
        bullet2->SetStartPosition(position + mvt);
        mvt = PE_Vec2{ 0.f, 2.3f };
        Bulletlaunch* bullet3 = new Bulletlaunch(m_scene, true, mvt);
        bullet3->SetStartPosition(position + mvt);
        mvt = PE_Vec2{ 2.3f, 2.3f };
        Bulletlaunch* bullet4 = new Bulletlaunch(m_scene, true, mvt);
        bullet4->SetStartPosition(position + mvt);
        mvt = PE_Vec2{ -2.3f, 2.3f };
        Bulletlaunch* bullet5 = new Bulletlaunch(m_scene, true, mvt);
        bullet5->SetStartPosition(position + mvt);
        m_timer_bigshoot = 0;

    }
    if (m_timer_shield < 300 && m_timer_shield>75) {
        SetShield(false);
    }
    if (m_timer_shield == 300) {
        SetShield(true);
        m_timer_shield = 0;
    }
    
}

void Boss::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();

    m_animator.Update(m_scene.GetTime());

    float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0 };
    rect.h = 2.0f * scale;
    rect.w = 2.0f * scale;
    
    camera->WorldToView(GetPosition()-PE_Vec2{0.f,0.6f}, rect.x, rect.y);
    m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
}

void Boss::OnRespawn()
{
    m_state = State::IDLE;
    m_isBounced = false;
    Reset_life();
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

void Boss::Damage(GameBody *damager)
{
    if (!m_shield) {
        if (heart_count <= 1) {
            SetEnabled(false);
            m_scene.setLevelEnded();
            m_scene.Quit();
        }
        if (Player* player = dynamic_cast<Player*>(damager)) {
            Remove_life();
        }
        else {
            Remove_life();
        }
    }
}

void Boss::OnCollisionStay(GameCollision &collision)
{
    
    PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;

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
    }
}

void Boss::Bounce(const PE_Vec2 &v)
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

void Boss::Remove_life() {
    heart_count--;
}

void Boss::SetCloser(bool res) {
    m_player_closer = res;
}

bool Boss::GetCloser() {
    return m_player_closer;
}

void Boss::SetShield(bool res) {
    m_shield = res;
}