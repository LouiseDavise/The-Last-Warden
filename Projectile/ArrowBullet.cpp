#include "ArrowBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"

ArrowBullet::ArrowBullet(float x, float y, float damage, const Engine::Point& direction, float rotation)
    : Projectile("Projectiles/BowArrow.png", 0.0f, damage, Engine::Point(x, y), direction, rotation, nullptr) {
    float speed = 700.0f;
    velocity = direction.Normalize() * speed;
}

void ArrowBullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);

    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    Engine::Point disp = velocity * deltaTime;
    Position = Position + disp;
    flightDist += disp.Magnitude();

    for (auto* obj : scene->EnemyGroup->GetObjects()) {
        auto* enemy = dynamic_cast<Enemy*>(obj);
        if (!enemy || !enemy->Visible) continue;

        if (Engine::Collider::IsCircleOverlap(Position, 4.0f, enemy->Position, enemy->CollisionRadius)) {
            enemy->Hit(damage);
            scene->ProjectileGroup->RemoveObject(objectIterator);
            return;
        }
    }

    if (flightDist >= maxDist) {
        scene->ProjectileGroup->RemoveObject(objectIterator);
        return;
    }
}
