#include "MageBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Collider.hpp"
#include "UI/Animation/CyanExplosion.hpp"
#include "UI/Animation/AreaEffect.hpp"
#include "Engine/AudioHelper.hpp"

MageBullet::MageBullet(float x, float y, float damage, const Engine::Point &direction, float rotation)
    : Projectile("Projectiles/MageBullet.png", 0.0f, damage, Engine::Point(x, y), direction, rotation, nullptr)
{
    float speed = 600.0f;
    velocity = direction.Normalize() * speed;
    AudioHelper::PlayAudio("mage-attack.mp3");
}

void MageBullet::Update(float deltaTime)
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
        scene->EffectGroup->AddNewObject(
            new AreaEffect(Position.x, Position.y, 96.0f, 0.5f, al_map_rgb(0, 184, 255)));

        scene->EffectGroup->AddNewObject(
            new CyanExplosion(Position.x, Position.y));
        return;
    }
}

void MageBullet::OnExplode()
{
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    const float radius = 96.0f;
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
                new AreaEffect(enemy->Position.x, enemy->Position.y, radius, 0.5f, al_map_rgb(0, 184, 255)));

            scene->EffectGroup->AddNewObject(
                new CyanExplosion(enemy->Position.x, enemy->Position.y));
        }
    }

    // Remove self from the projectile group
    scene->ProjectileGroup->RemoveObject(objectIterator);
}
