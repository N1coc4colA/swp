#include "LevelHeader.h"
#include "LevelScene.h"
#include "Image.h"
#include "Boss.h"
#include "Barreboss.h"
#include "Camera.h"
#include "Background.h"

LevelHeader::LevelHeader(LevelScene &scene):
    UIObject(scene), m_levelScene(scene), m_fireflyCount(nullptr), m_heartCount(nullptr)
{
    m_name = "LevelHeader";

    AssetManager &assets = scene.GetAssetManager();
    TTF_Font *font = assets.GetFont(FontID::LARGE);
    SDL_Color color = assets.GetColor(ColorID::NORMAL);

    int w, h;
    TTF_SizeUTF8(font, u8"00", &w, &h);
    float imgH = 80.0f;
    float imgW = imgH;
    float numW = (float)w;
    float sep = 20.0f;
    float panelW = imgW + sep + numW;
    float panelH = imgH;

    m_rect.anchorMin.Set(0.0f, 0.0f);
    m_rect.anchorMax.Set(0.0f, 0.0f);
    m_rect.offsetMin.Set(80.0f, 40.0f);
    m_rect.offsetMax = m_rect.offsetMin + Vec2(panelW, panelH);

    RE_Atlas *atlas = assets.GetAtlas(AtlasID::UI);

    float currX = 0.0f;
    float currY = 0.0f;

    // Image du nombre de luciolles
    RE_AtlasPart *part = atlas->GetPart("Firefly");
    AssertNew(part);
    Image *fireflyImage = new Image(scene, part, 0);
    fireflyImage->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    fireflyImage->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
    fireflyImage->GetLocalRect().offsetMin.Set(currX, currY);
    fireflyImage->GetLocalRect().offsetMax.Set(currX + imgW, currY + imgH);
    fireflyImage->SetParent(this);

    currX += imgW + sep;

    // Compteur du nombre de luciolles
    m_fireflyCount = new Text(scene, "0", font, color);
    m_fireflyCount->SetAnchor(RE_Anchor::WEST);
    m_fireflyCount->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    m_fireflyCount->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
    m_fireflyCount->GetLocalRect().offsetMin.Set(currX, currY);
    m_fireflyCount->GetLocalRect().offsetMax.Set(currX + numW, currY + imgH);
    m_fireflyCount->SetParent(this);

    
    currX = 0.0f;
    currY = 80.0f;
    // Image du nombre de coeur
    part = atlas->GetPart("Heart");
    AssertNew(part);
    Image* HeartImage = new Image(scene, part, 0);
    HeartImage->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    HeartImage->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
    HeartImage->GetLocalRect().offsetMin.Set(currX, currY);
    HeartImage->GetLocalRect().offsetMax.Set(currX + imgW, currY + imgH);
    HeartImage->SetParent(this);

    currX += imgW + sep;

    // Compteur du nombre de coeur
    m_heartCount = new Text(scene, "0", font, color);
    m_heartCount->SetAnchor(RE_Anchor::WEST);
    m_heartCount->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    m_heartCount->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
    m_heartCount->GetLocalRect().offsetMin.Set(currX, currY);
    m_heartCount->GetLocalRect().offsetMax.Set(currX + numW, currY + imgH);
    m_heartCount->SetParent(this);


    currX = 0.0f;
    currY = 920.0f;
    // Image du nombre de coeur
    part = atlas->GetPart("Life");
    AssertNew(part);
    Image* VieImage = new Image(scene, part, 0);
    VieImage->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    VieImage->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
    VieImage->GetLocalRect().offsetMin.Set(currX, currY);
    VieImage->GetLocalRect().offsetMax.Set(currX + imgW, currY + imgH);
    VieImage->SetParent(this);

    currX += imgW + sep;

    // Compteur du nombre de vie
    m_vieCount = new Text(scene, "0", font, color);
    m_vieCount->SetAnchor(RE_Anchor::WEST);
    m_vieCount->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
    m_vieCount->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
    m_vieCount->GetLocalRect().offsetMin.Set(currX, currY);
    m_vieCount->GetLocalRect().offsetMax.Set(currX + numW, currY + imgH);
    m_vieCount->SetParent(this);


    
    if (m_levelScene.GetBoss()) {
        Camera* camera = m_scene.GetActiveCamera();
        AssertNew(camera);

        SDL_Renderer* renderer = m_scene.GetRenderer();
        PE_AABB view = camera->GetWorldView();

        // Dimension du fond dans le référentiel monde
        float scale = camera->GetWorldToViewScale();
        PE_Vec2 worldDim = m_levelScene.worldDim;
        float layerW = scale * worldDim.x;
        float layerH = scale * worldDim.y;
        int coefpdv = m_levelScene.GetBoss()->Get_life();
        currX = 1740.0f;
        currY = 0.0f;
        // Image du nombre de coeur
        part = atlas->GetPart("Heart");
        AssertNew(part);
        //printf("%f %f\n", layerW + (10.f * scale) , layerH + 7.f * scale);
        Barreboss* HeartBarreboss = new Barreboss(scene, part, 0, &m_levelScene);
        HeartBarreboss->GetLocalRect().anchorMin.Set(0.0f, 0.0f);
        HeartBarreboss->GetLocalRect().anchorMax.Set(0.0f, 0.0f);
        HeartBarreboss->GetLocalRect().offsetMin.Set(currX,currY);
        HeartBarreboss->GetLocalRect().offsetMax.Set(currX+ imgW*coefpdv,currY + imgH);
        HeartBarreboss->SetParent(this);
    }



   
}

void LevelHeader::FixedUpdate()
{
    Player *player = m_levelScene.GetPlayer();

    m_fireflyCount->SetString(std::to_string(player->GetFireflyCount()));
    m_heartCount->SetString(std::to_string(player->GetHeartCount()));
    m_vieCount->SetString(std::to_string(player->GetLifeCount()));
    
    
}
