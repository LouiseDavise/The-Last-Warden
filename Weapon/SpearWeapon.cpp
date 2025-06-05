#include "SpearWeapon.hpp"
#include <cmath>
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/LOG.hpp"

using Engine::Point;

static constexpr float PI = 3.1415926f;

// weap_path, dmg, speed, startPos, owner
SpearWeapon::SpearWeapon(const Point& startPos, Player* owner)
    : Weapon("Weapons/Spear.png", 40.f, 750.f, startPos, owner)
{
    CollisionRadius = 0;
    Anchor          = Point(0.5f, 0.5f);
    Size.x = 80;
    Size.y = 80;
}

void SpearWeapon::Use(float tx, float ty)
{
    if (!available || isFlying) return;          // already thrown
    Point dir = Point(tx, ty) - Position;
    if (dir.Magnitude() < 1e-3f) return;            // zero-length guard

    dir        = dir.Normalize();
    velocity   = dir * speed;
    Rotation   = std::atan2(dir.y, dir.x);
    isFlying   = true;
    available  = false;
    flightDist = 0.f;
}

void SpearWeapon::Update(float dt)
{
    Sprite::Update(dt);
    if (isFlying) {
        Point disp   = velocity * dt;
        Position     = Position + disp;
        flightDist  += disp.Magnitude();
        TryHitEnemies();

        float mapW = PlayScene::MapWidth * PlayScene::BlockSize;
        float mapH = (PlayScene::MapHeight + 3) * PlayScene::BlockSize;  // +3 from PlayScene
        if (flightDist >= maxDistance || Position.x < 0 || Position.x > mapW || Position.y < 0 || Position.y > mapH) {
            isFlying = false;
            velocity = Point{0, 0};
            return;
        }
    } else {
        TryReclaim();          // lying on ground or already in hand
        Weapon::Update(dt);
    }
}

void SpearWeapon::TryHitEnemies()
{
    PlayScene* scene = GetPlayScene();
    if (!scene) return;

    float halfLen = Size.x * 0.5f;              // spear length  (texture width)
    float halfW   = std::max(4.0f, Size.y*0.25f); // and small thickness

    for (auto* obj : scene->EnemyGroup->GetObjects()) {
        auto* e = dynamic_cast<Enemy*>(obj);
        if (!e || !e->Visible) continue;

        if (PointInsideRotatedRect(e->Position, Position, halfLen, halfW, Rotation))
            e->Hit(damage);
    }
}

void SpearWeapon::TryReclaim()
{
    PlayScene* scene = GetPlayScene();
    if (!scene || !owner) return;

    const float pickR = 35.f;
    if ((owner->Position - Position).MagnitudeSquared() <= pickR*pickR) {
        Reclaim();                // base-class sets available=true and snaps to hand
        isFlying = false;
    }
}

bool SpearWeapon::PointInsideRotatedRect(const Point& p, const Point& c,
                                         float halfL, float halfW, float rotRad) const
{
    float cosR =  std::cos(-rotRad);
    float sinR =  std::sin(-rotRad);
    float dx   =  p.x - c.x;
    float dy   =  p.y - c.y;

    float lx =  dx * cosR - dy * sinR;
    float ly =  dx * sinR + dy * cosR;

    return std::abs(lx) <= halfL && std::abs(ly) <= halfW;
}
