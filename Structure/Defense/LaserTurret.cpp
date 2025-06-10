#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Projectile/LaserBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "LaserTurret.hpp"
#include "Scene/PlayScene.hpp"

const int LaserTurret::Price = 300;
LaserTurret::LaserTurret(float x, float y) : Turret("Structures/tower-base.png", "Structures/turret-2.png", x, y, 300, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void LaserTurret::CreateProjectile() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change Projectile position to the front of the gun barrel.
    getPlayScene()->ProjectileGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->ProjectileGroup->AddNewObject(new LaserBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}
