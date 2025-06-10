#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Projectile/Arrow.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "BowTower.hpp"
#include "Scene/PlayScene.hpp"

const int BowTower::Price = 100;
BowTower::BowTower(float x, float y) : Tower("Structures/tower-base.png", "Structures/BowTower.png", x, y, 300, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void BowTower::CreateProjectile() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change Projectile position to the front of the gun barrel.
    getPlayScene()->ProjectileGroup->AddNewObject(new Arrow(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->ProjectileGroup->AddNewObject(new Arrow(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}
