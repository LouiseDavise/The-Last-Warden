#include "WandWeapon.hpp"
#include <cmath>
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/IObject.hpp"
#include "Engine/LOG.hpp"
#include "Projectile/MagicBullet.hpp"

using Engine::Point;
static constexpr float PI = 3.1415926f;

WandWeapon::WandWeapon(const Point &startPos, Player *owner)
    : Weapon("Weapons/Wand.png", 25, 900.f, startPos, owner)
{
    CollisionRadius = 0;
    Anchor = Point(0.5f, 0.5f);
    Size.x = 64;
    Size.y = 64;
}

void WandWeapon::Use(float tx, float ty)
{
    if (!available || coolingDown || currentQuota <= 0)
        return;

    Engine::Point direction = Engine::Point(tx, ty) - Position;
    if (direction.Magnitude() < 1e-3f)
        return;

    direction = direction.Normalize();
    float rotation = std::atan2(direction.y, direction.x);

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (scene)
    {
        MagicBullet *bullet = new MagicBullet(Position.x, Position.y, damage, direction, rotation);
        scene->ProjectileGroup->AddNewObject(static_cast<Engine::IObject *>(bullet));

        if (!scene->IsMochiHealing())
        {
            currentQuota--;
            if (currentQuota == 0)
            {
                coolingDown = true;
                cooldownTimer = 0.0f;
            }
        }
    }
}

void WandWeapon::Update(float dt)
{
    Sprite::Update(dt);
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());

    if (coolingDown && !scene->IsMochiHealing())
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
        float mapH = (PlayScene::MapHeight + 3) * PlayScene::BlockSize;
        if (flightDist >= maxDistance || Position.x < 0 || Position.x > mapW || Position.y < 0 || Position.y > mapH)
        {
            isFlying = false;
            velocity = Point{0, 0};
            return;
        }
    }
    else
    {
        TryReclaim();
        Weapon::Update(dt);
    }
}

void WandWeapon::TryHitEnemies()
{
    PlayScene *scene = GetPlayScene();
    if (!scene)
        return;

    float halfLen = Size.x * 0.5f;
    float halfW = std::max(4.0f, Size.y * 0.25f);

    for (auto *obj : scene->EnemyGroup->GetObjects())
    {
        auto *e = dynamic_cast<Enemy *>(obj);
        if (!e || !e->Visible)
            continue;

        if (PointInsideRotatedRect(e->Position, Position, halfLen, halfW, Rotation))
            e->Hit(damage);
    }
}

void WandWeapon::TryReclaim()
{
    PlayScene *scene = GetPlayScene();
    if (!scene || !owner)
        return;

    const float pickR = 35.f;
    if ((owner->Position - Position).MagnitudeSquared() <= pickR * pickR)
    {
        Reclaim();
        isFlying = false;
    }
}

bool WandWeapon::PointInsideRotatedRect(const Point &p, const Point &c, float halfL, float halfW, float rotRad) const
{
    float cosR = std::cos(-rotRad);
    float sinR = std::sin(-rotRad);
    float dx = p.x - c.x;
    float dy = p.y - c.y;
    float lx = dx * cosR - dy * sinR;
    float ly = dx * sinR + dy * cosR;
    return std::abs(lx) <= halfL && std::abs(ly) <= halfW;
}
