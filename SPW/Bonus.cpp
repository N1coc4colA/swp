#include "Bonus.h"
#include "Scene.h"
#include "Camera.h"
#include "Heart.h"
#include "Firefly.h"
#include "Shield.h"

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
}

void Bonus::OnCollisionEnter(GameCollision &collision)
{
    
}


void Bonus::Give_Bonus() 
{
    if (m_active) {
<<<<<<< Updated upstream
        int Id_bonus = rand() % 2;
        
        switch (Id_bonus) {
=======
        int Id_bonus = rand() % 3;
        
        Shield* shields = new Shield(m_scene);
        shields->SetStartPosition(GetPosition());
        /*switch (Id_bonus) {
>>>>>>> Stashed changes
        case 0: {

            printf("here\n\n");
            Firefly* firefly = new Firefly(m_scene);
            firefly->SetStartPosition(GetPosition());
        }
        case 1: {
<<<<<<< Updated upstream
            Heart* heart = new Heart(m_scene);
            heart->SetStartPosition(GetPosition());
        }
        
        case 2 :
            Shield * shield = new Shield(m_scene);
            shield->SetStartPosition(GetPosition());
=======
            printf("la\n\n");
            //Firefly* firefly = new Firefly(m_scene);
            //firefly->SetStartPosition(GetPosition());
            Heart* heart = new Heart(m_scene);
            heart->SetStartPosition(GetPosition());
        }
        case 2: {

            printf("ici\n\n");
            //Shield* shield = new Shield(m_scene);
            //shield->SetStartPosition(GetPosition());
>>>>>>> Stashed changes
        }
        default: {
            break;
            //Shield* shields = new Shield(m_scene);
            //shields->SetStartPosition(GetPosition());
        }
        }*/
        Set_BonusEmpty();
    }
}

void Bonus::Set_BonusEmpty(){
    m_active = false;
    m_state = State::EMPTY;
    m_animator.PlayAnimation("EMPTY");
}