#include "EnemyCircular.hpp"
#include "Engine/Point.hpp"
#include "Engine/Collider.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Structure/Structure.hpp"
#include "Player/Player.hpp"

EnemyCircular::EnemyCircular(float x, float y, float vx, float vy, float damage, float radius)
    : Projectile("Projectiles/Enemy_bullet.png", std::sqrt(vx * vx + vy * vy), damage, Engine::Point(x, y), Engine::Point(vx, vy).Normalize(), 0, nullptr) {
    CollisionRadius = radius;
    Velocity = Engine::Point(vx, vy);
}

void EnemyCircular::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();

    // Explode if it hits player
    Player* player = scene->GetPlayer();
    if (player && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, player->Position, player->CollisionRadius)) {
        player->Hit(damage, Position);
        OnExplode(nullptr);  // You can pass nullptr or implement custom behavior
        return;
    }

    // Explode if it hits any structure
    for (auto& it : scene->StructureGroup->GetObjects()) {
        Structure* s = dynamic_cast<Structure*>(it);
        if (s && s->GetHP() > 0 && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, s->Position, s->CollisionRadius)) {
            s->Hit(damage);
            OnExplode(nullptr);
            return;
        }
    }

    // Remove if out of bounds
    if (Position.x < 0 || Position.x > scene->MapWidth * PlayScene::BlockSize ||
        Position.y < 0 || Position.y > scene->MapHeight * PlayScene::BlockSize) {
        OnExplode(nullptr);
    }
}

void EnemyCircular::OnExplode(Enemy* enemy) {
    getPlayScene()->ProjectileGroup->RemoveObject(objectIterator);
}
