#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Projectile/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "MachineGunTurret.hpp"
#include "Scene/PlayScene.hpp"

const int MachineGunTurret::Price = 50;
MachineGunTurret::MachineGunTurret(float x, float y)
    : Tower("Structures/tower-base.png", "Structures/turret-1.png", x, y, 200, Price, 0.4) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void MachineGunTurret::CreateProjectile() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change Projectile position to the front of the gun barrel.
    getPlayScene()->ProjectileGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}
