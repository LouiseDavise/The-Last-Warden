#include "EnemyCircular.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"

EnemyCircular::EnemyCircular(float x, float y, float damage, const Engine::Point &direction, float rotation, float maxDist)
    : Projectile("Projectiles/Enemy_bullet.png", 0.0f, damage, Engine::Point(x, y), direction, rotation, nullptr),
      maxDist(maxDist)
{
    velocity = direction.Normalize() * speed;
}

void EnemyCircular::Update(float deltaTime)
{
    Sprite::Update(deltaTime);

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    Engine::Point displacement = velocity * deltaTime;
    Position = Position + displacement;
    flightDist += displacement.Magnitude();

    for (auto *obj : scene->EnemyGroup->GetObjects())
    {
        auto *enemy = dynamic_cast<Enemy *>(obj);
        if (!enemy || !enemy->Visible)
            continue;

        if (Engine::Collider::IsCircleOverlap(Position, 8.0f, enemy->Position, enemy->CollisionRadius))
        {
            enemy->Hit(damage);
            scene->ProjectileGroup->RemoveObject(objectIterator);
            return;
        }
    }

    if (flightDist >= maxDist)
    {
        scene->ProjectileGroup->RemoveObject(objectIterator);
        return;
    }
}
