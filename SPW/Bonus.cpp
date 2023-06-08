#include "Bonus.h"
#include "Scene.h"
#include "Camera.h"
#include "Heart.h"
#include "Firefly.h"
#include "Shield.h"


#include <cstdlib>
#include <random>


Bonus::Bonus(Scene &scene) :
    GameBody(scene, Layer::TERRAIN_FOREGROUND), m_animator(), m_hit(false)
{
    m_name = "BonusFull";
    RE_Atlas* atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    AssertNew(atlas)
        RE_AtlasPart* part = atlas->GetPart("BonusFull");
    AssertNew(part)
        RE_TexAnim* anim = new RE_TexAnim(m_animator, "FULL", part);
    anim->SetCycleCount(0);

    atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    AssertNew(atlas)
    part = atlas->GetPart("BonusEmpty");
    AssertNew(part)
    RE_TexAnim* emptyanim = new RE_TexAnim(m_animator, "EMPTY", part);
    emptyanim->SetCycleCount(0);

}

void Bonus::Start()
{
    SetToRespawn(true);

    // Joue l'animation par défaut
    m_animator.PlayAnimation("FULL");

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
    PE_Collider *collider = body->CreateCollider(colliderDef);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simulés
    body->SetAwake(false);

}

void Bonus::Render()
{
    SDL_Renderer* renderer = m_scene.GetRenderer();
    const Camera* camera = m_scene.GetActiveCamera();

    if (m_active)
    {
        
        m_animator.Update(m_scene.GetTime());

        const float scale = camera->GetWorldToViewScale();
        SDL_FRect rect = { 0 };
        rect.w = scale;
        rect.h = scale;
        camera->WorldToView(GetPosition(), rect.x, rect.y);
        m_animator.RenderCopyF(&rect, RE_Anchor::SOUTH);
    }
    else {
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
    m_active = true;
    m_state = State::FULL;
    m_animator.PlayAnimation("FULL");
}

void Bonus::OnCollisionEnter(GameCollision &collision)
{
    
}


void Bonus::Give_Bonus() 
{
    if (m_active)
    {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> distr(0, 2);
        int Id_bonus = distr(gen);
        
        //printf("%d\n", Id_bonus);
        if(Id_bonus==0) {
                //printf("ici\n");
                Heart* heart = new Heart(m_scene);
                heart->SetStartPosition(GetPosition());
        }
        else if(Id_bonus == 1) {
            //printf("here\n");
            Shield * shield = new Shield(m_scene);
            shield->SetStartPosition(GetPosition());
         }
        else if (Id_bonus == 2) {
            //printf("where\n");
            Firefly* firefly = new Firefly(m_scene);
            firefly->SetStartPosition(GetPosition());
        }
            
        Set_BonusEmpty();
        
    }
}

void Bonus::Set_BonusEmpty()
{
    m_active = false;
    m_state = State::EMPTY;
    m_animator.PlayAnimation("EMPTY");
}