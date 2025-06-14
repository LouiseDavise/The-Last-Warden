#include "SpearWeapon.hpp"
#include <cmath>
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/LOG.hpp"
#include "UI/Animation/ClashEffect.hpp"
#include "UI/Animation/AreaEffect.hpp"

using Engine::Point;

static constexpr float PI = 3.1415926f;

// weap_path, dmg, speed, startPos, owner
SpearWeapon::SpearWeapon(const Point &startPos, Player *owner)
    : Weapon("Weapons/Spear.png", 30, 750.f, startPos, owner)
{
    CollisionRadius = 0;
    Anchor = Point(0.5f, 0.5f);
    Size.x = 80;
    Size.y = 80;
}

void SpearWeapon::Use(float tx, float ty)
{
    // Usability check
    if (!available || isFlying || coolingDown || currentQuota <= 0)
        return;

    Point dir = Point(tx, ty) - Position;
    if (dir.Magnitude() < 1e-3f)
        return;

    dir = dir.Normalize();
    velocity = dir * speed;
    Rotation = std::atan2(dir.y, dir.x);
    isFlying = true;
    available = false;
    flightDist = 0.f;
    hitEnemies.clear(); // Clear hit record for new throw
    // Only reduce quota and start cooldown IF not healing
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!(scene && scene->IsMochiHealing()))
    {
        currentQuota--;
        if (currentQuota == 0)
        {
            coolingDown = true;
            cooldownTimer = 0.0f;
        }
    }
}

void SpearWeapon::Update(float dt)
{
    Sprite::Update(dt);
    if (coolingDown)
    {
        cooldownTimer += dt;
        if (cooldownTimer >= cooldownTime)
        {
            cooldownTimer = 0;
            currentQuota = maxQuota;
            coolingDown = false;
        }
    }
    if (isFlying)
    {
        Point disp = velocity * dt;
        Position = Position + disp;
        flightDist += disp.Magnitude();
        TryHitEnemies();

        float mapW = PlayScene::MapWidth * PlayScene::BlockSize;
        float mapH = (PlayScene::MapHeight + 3) * PlayScene::BlockSize; // +3 from PlayScene
        if (flightDist >= maxDistance || Position.x < 0 || Position.x > mapW || Position.y < 0 || Position.y > mapH)
        {
            isFlying = false;
            velocity = Point{0, 0};
            return;
        }
    }
    else
    {
        TryReclaim(); // lying on ground or already in hand
        Weapon::Update(dt);
    }
}

void SpearWeapon::TryHitEnemies()
{
    PlayScene *scene = GetPlayScene();
    if (!scene)
        return;

    float halfLen = Size.x * 0.5f;                // spear length  (texture width)
    float halfW = std::max(4.0f, Size.y * 0.25f); // and small thickness

    for (auto *obj : scene->EnemyGroup->GetObjects())
    {
        auto *e = dynamic_cast<Enemy *>(obj);
        if (!e || !e->Visible)
            continue;
        if (hitEnemies.count(e)) continue;

        if (PointInsideRotatedRect(e->Position, Position, halfLen, halfW, Rotation)){
            e->Hit(damage);
            hitEnemies.insert(e);
            scene->EffectGroup->AddNewObject(new ClashEffect(e->Position.x, e->Position.y));
            scene->EffectGroup->AddNewObject(new AreaEffect(Position.x, Position.y, 64.0f, 0.5f, al_map_rgb(255,244,79)));
        }
    }  
}

void SpearWeapon::TryReclaim()
{
    PlayScene *scene = GetPlayScene();
    if (!scene || !owner)
        return;

    const float pickR = 35.f;
    if ((owner->Position - Position).MagnitudeSquared() <= pickR * pickR)
    {
        Reclaim(); // base-class sets available=true and snaps to hand
        isFlying = false;
    }
}

bool SpearWeapon::PointInsideRotatedRect(const Point &p, const Point &c, float halfL, float halfW, float rotRad) const
{
    float cosR = std::cos(-rotRad);
    float sinR = std::sin(-rotRad);
    float dx = p.x - c.x;
    float dy = p.y - c.y;
    float lx = dx * cosR - dy * sinR;
    float ly = dx * sinR + dy * cosR;
    return std::abs(lx) <= halfL && std::abs(ly) <= halfW;
}
