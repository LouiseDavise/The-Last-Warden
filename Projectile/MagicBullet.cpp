#include "MagicBullet.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"

MagicBullet::MagicBullet(float x, float y, float damage, const Engine::Point& direction, float rotation)
    : Projectile("Projectiles/MagicBullet.png", 600.0f, damage, Engine::Point(x, y), direction, rotation, nullptr) {
}
