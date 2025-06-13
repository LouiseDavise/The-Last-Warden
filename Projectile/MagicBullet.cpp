#include "MagicBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Collider.hpp"
#include "UI/Animation/AreaEffect.hpp"

MagicBullet::MagicBullet(float x, float y, float damage, const Engine::Point &direction, float rotation)
    : Projectile("Projectiles/MagicBullet.png", 0.0f, damage, Engine::Point(x, y), direction, rotation, nullptr)
{
    float speed = 600.0f;
    velocity = direction.Normalize() * speed;
}

void MagicBullet::Update(float deltaTime)
{
    Sprite::Update(deltaTime);

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    // Move the bullet
    Engine::Point displacement = velocity * deltaTime;
    Position = Position + displacement;
    flightDist += displacement.Magnitude();

    // Hit enemy → explode
    for (auto *obj : scene->EnemyGroup->GetObjects())
    {
        auto *enemy = dynamic_cast<Enemy *>(obj);
        if (!enemy || !enemy->Visible)
            continue;

        if (Engine::Collider::IsCircleOverlap(Position, 8.0f, enemy->Position, enemy->CollisionRadius))
        {
            OnExplode();
            return;
        }
    }

    // Reached max distance → explode
    if (flightDist >= maxDist)
    {
        OnExplode();
        return;
    }
}

void MagicBullet::OnExplode()
{
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    const float radius = 64.0f;
    for (auto *obj : scene->EnemyGroup->GetObjects())
    {
        auto *enemy = dynamic_cast<Enemy *>(obj);
        if (!enemy || !enemy->Visible)
            continue;

        float distSqr = (enemy->Position - Position).MagnitudeSquared();
        if (distSqr <= radius * radius)
        {
            enemy->Hit(damage);
            scene->EffectGroup->AddNewObject(
                new AreaEffect(enemy->Position.x, enemy->Position.y, radius, 0.5f));

        }
    }

    // Remove self from the projectile group
    scene->ProjectileGroup->RemoveObject(objectIterator);
}
