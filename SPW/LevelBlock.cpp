#include "LevelBlock.h"
#include "Scene.h"
#include "Camera.h"
#include "HubPlayer.h"
#include "Graphics.h"


LevelBlock::LevelBlock(Scene &scene, Tile::Type tt)
    : GameBody(scene, Layer::TERRAIN)
    , m_tileType(tt)
{
    m_name = "LevelBlock";
    
    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::WORLD);
    AssertNew(atlas)

    switch (tt)
    {
    case Tile::Type::GENTLE_SLOPE_L1:
        {
            m_part = atlas->GetPart("Demo");
            break;
        }
    case Tile::Type::GENTLE_SLOPE_L2:
        {
            m_part = atlas->GetPart("Mountains");
            break;
        }
    case Tile::Type::GENTLE_SLOPE_R1:
        {
            m_part = atlas->GetPart("Desert");
            break;
        }
    case Tile::Type::GENTLE_SLOPE_R2:
        {
            m_part = atlas->GetPart("Where");
            break;
        }
    default:
        {
            abort();
        }
    }
    AssertNew(m_part)
    
    m_whatPart = atlas->GetPart("What");
    AssertNew(m_whatPart)

    text = new RE_Text(m_scene.GetRenderer(), m_scene.GetAssetManager().GetFont(FontID::NORMAL), ".", m_scene.GetAssetManager().GetColor(ColorID::BLACK));
}

void LevelBlock::Start()
{
    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::STATIC;
    bodyDef.position = GetStartPosition();//+ PE_Vec2(.f, 0.0f);
    bodyDef.name = "LevelBlock";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    // Crée le collider
    PE_PolygonShape box(0.f, 0.f, 0.f, 1.f);
    PE_ColliderDef colliderDef;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.filter.maskBits = CATEGORY_PLAYER;
    colliderDef.shape = &box;
    PE_Collider *collider = body->CreateCollider(colliderDef);
}

void LevelBlock::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    const Camera *camera = m_scene.GetActiveCamera();

    const float scale = camera->GetWorldToViewScale();
    
    SDL_FRect rect = { 0, 0, scale, scale};
    camera->WorldToView(GetPosition(), rect.x, rect.y);
    m_part->RenderCopyF(0, &rect, RE_Anchor::SOUTH_WEST); // SOUTH_WEST
    
    rect = { 0, 0, scale, scale};
    camera->WorldToView(GetPosition() - PE_Vec2{0.f, 1.f}, rect.x, rect.y);
    m_whatPart->RenderCopyF(0, &rect, RE_Anchor::SOUTH_WEST);

    if (showDescription)
    {
        rect = { 0, 0, scale * 3.f, scale};
        camera->WorldToView(GetPosition() + PE_Vec2{0.f, 1.f}, rect.x, rect.y);
        RE_RenderCopyF(renderer, text->GetTexture(), nullptr, &rect, RE_Anchor::SOUTH_WEST);
    }
}

void LevelBlock::FixedUpdate()
{
    const PE_Vec2 position = GetPosition();
    
    RayHit h1 = m_scene.RayCast(position + PE_Vec2{-0.f, -0.2f}, PE_Vec2{1.4f, 0.f}, 1.f, CATEGORY_PLAYER, false);
    
    RayHit h2 = m_scene.RayCast(position + PE_Vec2{-0.2f, -0.2f}, PE_Vec2{0.f, 1.4f}, 1.f, CATEGORY_PLAYER, false);
    RayHit h3 = m_scene.RayCast(position + PE_Vec2{-0.2f, 1.4f}, PE_Vec2{1.4f, 0.f}, 1.f, CATEGORY_PLAYER, false);
    RayHit h4 = m_scene.RayCast(position + PE_Vec2{1.4f, -0.2f}, PE_Vec2{0.f, 1.4f}, 1.f, CATEGORY_PLAYER, false);
    
    RayHit h5 = m_scene.RayCast(position + PE_Vec2{1.f, 0.f}, PE_Vec2{-1.f, -1.f}, 1.f, CATEGORY_PLAYER, false);
    RayHit h6 = m_scene.RayCast(position, PE_Vec2{1.f, -1.f}, 1.f, CATEGORY_PLAYER, false);

    if (h1.collider)
    {
        trigger();
        return;
    } else if (h6.collider || h5.collider)
    {
        showDescription = true;
    } else if (h2.collider || h3.collider || h4.collider) {
        if (HubPlayer *player = dynamic_cast<HubPlayer *>(h2.collider ? h2.gameBody : (h3.collider ? h3.gameBody : h4.gameBody)))
        {
            player->GetBody()->SetPosition(position + PE_Vec2{0.5f, -1.5f});
        }
    } else
    {
        showDescription = false;
    }
}

void LevelBlock::DrawGizmos()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Graphics graphics(renderer, *m_scene.GetActiveCamera());
    const PE_Vec2 position = GetBody()->GetPosition();
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    graphics.DrawVector(PE_Vec2{1.4f, 0.f}, position + PE_Vec2{-0.2f, -0.2f});
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    graphics.DrawVector(PE_Vec2{0.f, 1.4f}, position + PE_Vec2{-0.2f, -0.2f});
    graphics.DrawVector(PE_Vec2{1.4f, 0.f}, position + PE_Vec2{-0.2f, 1.2f});
    graphics.DrawVector(PE_Vec2{0.f, 1.4f}, position + PE_Vec2{1.2f, -0.2f});
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    graphics.DrawVector(PE_Vec2{-1.f, -1.f}, position + PE_Vec2{1.f, 0.f});
    graphics.DrawVector(PE_Vec2{1.f, -1.f}, position);
}

void LevelBlock::OnRespawn()
{
    SetToRespawn(true);
    SetBodyEnabled(true);
    SetEnabled(true);
    
    PE_Body *body = GetBody();
    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);
    body->ClearForces();
}

void LevelBlock::OnCollisionStay(GameCollision &collision)
{
    if (collision.otherCollider->CheckCategory(CATEGORY_PLAYER))
    {
        collision.ResolveUp();
        collision.SetEnabled(false);
        trigger();
        GetBody()->SetPosition(GetStartPosition());
    }
}

void LevelBlock::trigger()
{
    m_scene.SetLevelID(m_id);
    m_scene.usesTrick = true;
    m_scene.tricked = m_id;
    m_scene.Quit();
}

void LevelBlock::SetId(int i)
{
    m_id = i;
    m_scene.requiresLevelLoad = true;
    if (text)
    {
        const char *strings[4] = {
          u8"Monde démo",
            "Monde montagneux",
          u8"Monde désertique",
            "Monde perdu"
        };
        text->SetString(strings[i]);
    }
}

