#include "Player.h"
#include "Camera.h"
#include "Scene.h"
#include "Collectable.h"
#include "Enemy.h"
#include "Graphics.h"
#include  "Brick.h"


Player::Player(Scene &scene)
	: GameBody(scene, Layer::PLAYER)
	, m_animator()
{
    m_name = "Player";

    SetToRespawn(true);

    AssetManager &assetsManager = scene.GetAssetManager();
    RE_Atlas *atlas = assetsManager.GetAtlas(AtlasID::PLAYER);
    RE_AtlasPart *part = nullptr;

    // Animation "Idle"
    part = atlas->GetPart("Idle");
    AssertNew(part);
    RE_TexAnim *idleAnim = new RE_TexAnim(
        m_animator,
        "Idle",
        part
    );
    idleAnim->SetCycleCount(0);

    part = atlas->GetPart("Falling");
    AssertNew(part);
	RE_TexAnim *fallingAnim = new RE_TexAnim(
        m_animator,
		"Falling",
		part
	);
    fallingAnim->SetCycleCount(-1);
	fallingAnim->SetCycleTime(0.2f);

    part = atlas->GetPart("Running");
    AssertNew(part);
    RE_TexAnim *runningAnim = new RE_TexAnim(
        m_animator,
		"Running",
		part
	);
    runningAnim->SetCycleCount(-1);
    runningAnim->SetCycleTime(1.f);

    part = atlas->GetPart("Dying");
    AssertNew(part);
    RE_TexAnim *dyingAnim = new RE_TexAnim(
        m_animator,
		"Dying",
		part
	);
    dyingAnim->SetCycleCount(0);
    dyingAnim->SetCycleTime(1.f);

    part = atlas->GetPart("Skidding");
    AssertNew(part);
    RE_TexAnim *skiddingAnim = new RE_TexAnim(
        m_animator,
		"Skidding", 
		part
	);
    skiddingAnim->SetCycleCount(0);

    // Couleur des colliders en debug
    m_debugColor.r = 255;
    m_debugColor.g = 0;
    m_debugColor.b = 0;
}

Player::~Player()
{
}

void Player::Start()
{
    
    // Joue l'animation par défaut
    m_animator.PlayAnimation("Idle");

    // Crée le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.5f);
    bodyDef.name = "Player";
    bodyDef.damping.SetZero();
	bodyDef.mass = 4.f;
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    // Création du collider
    PE_ColliderDef colliderDef;

    PE_CapsuleShape capsule(PE_Vec2(0.0f, 0.35f), PE_Vec2(0.0f, 0.85f), 0.35f);
//PE_Vec2(-0.3f, 0.1f), PE_Vec2(0.2f, 0.1f), 0.7f);
    colliderDef.friction = 1.f;
    colliderDef.filter.categoryBits = CATEGORY_PLAYER;
    //colliderDef.filter.maskBits = CATEGORY_TERRAIN;
    colliderDef.shape = &capsule;
    PE_Collider *collider = body->CreateCollider(colliderDef);
}

void Player::Update()
{
    ControlsInput &controls = m_scene.GetInputManager().GetControls();

    // Sauvegarde les contrôles du joueur pour modifier
    // sa physique au prochain FixedUpdate()
    
	if (!m_jump) {
        m_jump = controls.jumpPressed;
	}

    m_hDirection = controls.hAxis;
}

void Player::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();

    // Met à jour les animations du joueur
    m_animator.Update(m_scene.GetTime());

    float scale = camera->GetWorldToViewScale();
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_FRect rect = { 0 };

    rect.h = 1.4f * scale;
    rect.w = 2.f * scale;
    PE_Vec2 pos = GetPosition();
	//pos.y += 0.1f;
    camera->WorldToView(pos, rect.x, rect.y);

    // Dessine l'animation du joueur
    m_animator.RenderCopyExF(&rect, RE_Anchor::SOUTH, 0.f, Vec2(0.5f, 0.5f), flip);
}

void Player::FixedUpdate()
{
    PE_Body *body = GetBody();
    const PE_Vec2 position = body->GetPosition();

    // Réveille les corps autour du joueur
    WakeUpSurroundings();

    // Tue le joueur s'il tombe dans un trou
    if (position.y < -2.0f)
    {
        m_scene.Respawn();
        return;
    }

    //--------------------------------------------------------------------------
    // Détection du sol

    bool m_onGround = false;
    PE_Vec2 gndNormal = PE_Vec2::up;

    // Lance deux rayons vers le bas ayant pour origines
    // les coins gauche et droit du bas du collider du joueur
    // Ces deux rayons sont dessinés en jaune dans DrawGizmos()
    PE_Vec2 originL = position + PE_Vec2(-0.35f, 0.0f);
    PE_Vec2 originR = position + PE_Vec2(+0.35f, 0.0f);

    // Les rayons ne touchent que des colliders solides (non trigger)
    // ayant la catégorie FILTER_TERRAIN
    RayHit hitL = m_scene.RayCast(originL, PE_Vec2::down, 0.1f, CATEGORY_TERRAIN, true);
    RayHit hitR = m_scene.RayCast(originR, PE_Vec2::down, 0.1f, CATEGORY_TERRAIN, true);

    if (hitL.collider != NULL)
    {
        // Le rayon gauche à touché le sol
        m_onGround = true;
        gndNormal = hitL.normal;
    }
    if (hitR.collider != NULL)
    {
        // Le rayon droit à touché le sol
        m_onGround = true;
        gndNormal = hitR.normal;
    }

    //--------------------------------------------------------------------------
    // Etat du joueur

    // Détermine l'état du joueur et change l'animation si nécessaire
	State tmpState = m_state;
    
	if (m_onGround) {
	    if (m_jump) {
	        tmpState = State::SKIDDING;
	        m_jump = false;
	    } else if (m_hDirection == 0.f)
	    {
	        tmpState = State::IDLE;
	    } else {
			tmpState = State::RUNNING;
		}
    } else {
        if (tmpState != State::FALLING)
        {
            tmpState = State::FALLING;
        }
        if (m_jump)
        {
            const RayHit result1 = m_scene.RayCast(GetPosition(), PE_Vec2::up, 4, CATEGORY_TERRAIN, true);
            if (!result1.collider)
            {
                tmpState = State::SKIDDING;
            }
        }
    }

	if (m_state == State::DYING) {
		tmpState = State::DEAD;
	} else if (m_lifeCount <= 0) {
		tmpState = State::DYING;
	}

    if (m_state != tmpState)
    {
        switch (tmpState) {
        case State::SKIDDING: {
                body->ApplyImpulse(PE_Vec2::up * 80.f);
                m_animator.PlayAnimation("Skidding");
	            m_jump = false;
                break;
        }
        case State::FALLING: {
                m_animator.PlayAnimation("Falling");
                break;
        }
        case State::IDLE: {
                m_animator.PlayAnimation("Idle");
                break;
        }
        case State::RUNNING: {
                m_animator.PlayAnimation("Running");
                break;
        }
        case State::DYING: {
                m_animator.PlayAnimation("Dying");
                break;
        }
        default: {
                break;
        }
        }
        m_state = tmpState;
    }

    // Orientation du joueur
    m_facingRight = true;
    if (m_hDirection < 0.f)
    {
        m_facingRight = false;
    } else if (m_hDirection > 0.f)
    {
        m_facingRight = true;
    }

    //--------------------------------------------------------------------------
    // Modification de la vitesse et application des forces

    // Application des forces
    // Définit la force d'accélération horizontale du joueur
    PE_Vec2 direction = PE_Vec2::right;

    PE_Vec2 force = (80.0f * m_hDirection) * direction;
    body->ApplyForce(force);

    PE_Vec2 mvt = body->GetLocalVelocity();
    mvt.x = PE_Clamp(mvt.x, -20.f, 20.f);
    body->SetVelocity(mvt);
    
    const RayHit result1 = m_scene.RayCast(GetPosition(), PE_Vec2::up, 1.3f, CATEGORY_TERRAIN, false);
    if (result1.collider)
    {
        if (result1.collider->GetUserData().id == 2)
        {
            std::cout << "result!" << std::endl;
            if (Brick *brick = dynamic_cast<Brick *>(result1.gameBody))
            {
                std::cout << "Cast!" << std::endl;
                const float angleUp = PE_AngleDeg(result1.normal, PE_Vec2::up);
                if (angleUp <= 55.f)
                {
                    brick->touchedFromBottom();
                }
            }
        }
    }

    // TODO : Rebond sur les ennemis

    // Remarques :
    // Le facteur de gravité peut être modifié avec l'instruction
    // -> body->SetGravityScale(0.5f);
    // pour faire des sauts de hauteurs différentes.
    // La physique peut être différente si le joueur touche ou non le sol.

    // Définit la nouvelle vitesse du corps
    // TODO : Appliquer la nouvelle velocité au player
    
    if (timer_start) {
        timer_shield += m_scene.GetFixedTimeStep();
        if (timer_shield >= 2.f) {
            SetShield(false);
            timer_shield = 0;
            timer_start = false;
        }
    }
}

void Player::OnRespawn()
{
    PE_Body *body = GetBody();
    AssertNew(body);

    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);

    m_heartCount = 2;
    m_state = State::IDLE;
    m_hDirection = 0.0f;
    m_facingRight = true;
    m_bounce = false;
    m_jump = false;

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Idle");

    SetToRespawn(true);
}

void Player::DrawGizmos()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Graphics graphics(renderer, *m_scene.GetActiveCamera());
    const PE_Vec2 position = GetPosition();
    const PE_Vec2 velocity = GetVelocity();

    
    //const RayHit result1 = m_scene.RayCast(, 1, CATEGORY_TERRAIN, true);

    // Dessine en blanc le vecteur vitesse du joueur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    graphics.DrawVector(0.5f * velocity, position);

    // Dessine en jaune les rayons pour la détection du sol
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    const PE_Vec2 originL = position + PE_Vec2(-0.35f, 0.0f);
    const PE_Vec2 originR = position + PE_Vec2(+0.35f, 0.0f);
    graphics.DrawVector(0.1f * PE_Vec2::down, originL);
    graphics.DrawVector(0.1f * PE_Vec2::down, originR);
    graphics.DrawVector(PE_Vec2::up * 1.3f, position);
}

void Player::OnCollisionEnter(GameCollision &collision)
{
    const PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;

    // Collision avec un ennemi
    if (otherCollider->CheckCategory(CATEGORY_ENEMY))
    {
        Enemy *enemy = dynamic_cast<Enemy *>(collision.gameBody);
        if (enemy == nullptr)
        {
            assert(false);
            return;
        }

        // Calcule l'angle entre la normale de contact et le vecteur "UP"
        // L'angle vaut :
        // * 0 si le joueur est parfaitement au dessus de l'ennemi,
        // * 90 s'il est à gauche ou à droite
        // * 180 s'il est en dessous
        float angle = PE_AngleDeg(manifold.normal, PE_Vec2::up);
        if (angle < PLAYER_DAMAGE_ANGLE)
        {
            enemy->Damage(this);
            collision.ResolveUp();
        }
        else
        {
            collision.SetEnabled(false);
        }

        return;
    }

    // Collision avec un objet
    if (otherCollider->CheckCategory(CATEGORY_COLLECTABLE))
    {
        Collectable *collectable = dynamic_cast<Collectable *>(collision.gameBody);
        if (collectable == nullptr) return;

        // Collecte l'objet
        // C'est ensuite l'objet qui affecte un bonus au joueur,
        // en appellant AddFirefly() par exemple.
        
        collectable->Collect(this);
        return;
    }
}

void Player::OnCollisionStay(GameCollision &collision)
{
    const PE_Manifold &manifold = collision.manifold;
    const PE_Collider *otherCollider = collision.otherCollider;

    if (otherCollider->CheckCategory(CATEGORY_COLLECTABLE))
    {
        // Désactive la collision avec un objet
        // Evite d'arrêter le joueur quand il prend un coeur
        collision.SetEnabled(false);
        return;
    }
    else if (otherCollider->CheckCategory(CATEGORY_TERRAIN))
    {
        const float angleUp = PE_AngleDeg(manifold.normal, PE_Vec2::up);
        if (angleUp <= 55.f)
        {
            // Résoud la collision en déplaçant le joueur vers le haut
            // Evite de "glisser" sur les pentes si le joueur ne bouge pas
            collision.ResolveUp();
        } else if (Brick *brick = dynamic_cast<Brick *>(collision.gameBody))
        {
            brick->touchedFromBottom();
        }
    }
}

void Player::AddFirefly(int count)
{
    m_fireflyCount += count;
    if (m_fireflyCount % 5 == 0) {
        AddHeart();
    }
    
}

void Player::AddHeart()
{
    m_heartCount++;
}

void Player::Damage()
{
    if (!shield) {
        if (m_state == State::DYING) {
            Kill();
        }
        else if (m_heartCount == 1) {
            m_lifeCount--;
            Kill();
        }
        else {
            m_heartCount--;
            SetShield(true);
            timer_start = true;
            //timer_shield = 5; // plus que 2 direct shield


        }
    }
}

void Player::Kill()
{
    m_scene.Respawn();
}

class WakeUpCallback : public PE_QueryCallback
{
public:
    WakeUpCallback() {};
    virtual bool ReportCollider(PE_Collider *collider)
    {
        collider->GetBody()->SetAwake(true);
        return true;
    }
};

void Player::WakeUpSurroundings()
{
    PE_World &world = m_scene.GetWorld();
    PE_Vec2 position = GetBody()->GetPosition();
    PE_AABB aabb(
        position.x - 20.0f, position.y - 10.0f,
        position.x + 20.0f, position.y + 10.0f
    );
    WakeUpCallback callback;
    world.QueryAABB(callback, aabb);
}

