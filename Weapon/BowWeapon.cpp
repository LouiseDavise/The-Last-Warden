#include "BowWeapon.hpp"
#include <allegro5/allegro_primitives.h>
#include "Scene/PlayScene.hpp"
#include "Projectile/ArrowBullet.hpp"
#include "Engine/GameEngine.hpp"

using Engine::Point;

BowWeapon::BowWeapon(const Point &startPos, Player *owner)
    : Weapon("Weapons/Bow.png", 30, 800.0f, startPos, owner)
{
    Anchor = Point(0.5f, 0.5f);
    Size.x = 64;
    Size.y = 64;
}

void BowWeapon::Use(float tx, float ty)
{
    if (!available || coolingDown || currentQuota <= 0)
        return;

    Point direction = Point(tx, ty) - Position;
    if (direction.Magnitude() < 1e-3f)
        return;

    direction = direction.Normalize();
    float rotation = std::atan2(direction.y, direction.x);

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (scene)
    {
        ArrowBullet *bullet = new ArrowBullet(Position.x, Position.y, damage, direction, rotation, maxDistance);
        scene->ProjectileGroup->AddNewObject(static_cast<Engine::IObject *>(bullet));

        if (!scene->IsMochiHealing())
        {
            currentQuota--;
            if (currentQuota == 0)
            {
                coolingDown = true;
                cooldownTimer = 0.0f;
            }
        }
    }
}

void BowWeapon::Update(float dt)
{
    Sprite::Update(dt);

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (coolingDown && !scene->IsMochiHealing())
    {
        cooldownTimer += dt;
        if (cooldownTimer >= cooldownTime)
        {
            cooldownTimer = 0;
            currentQuota = maxQuota;
            coolingDown = false;
        }
    }

    Weapon::Update(dt);
}

void BowWeapon::Draw() const
{
    if (available && owner)
    {
        auto scene = GetPlayScene();
        if (scene)
        {
            Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();
            Engine::Point worldMouse = mouse + scene->camera;

            Point dir = worldMouse - Position;
            if (dir.Magnitude() >= 1e-3f)
            {
                Point end = Position + dir.Normalize() * maxDistance;
                al_draw_line(Position.x, Position.y, end.x, end.y, al_map_rgb(237, 255, 181), 0.2f);
            }
        }
    }
    Weapon::Draw();
}
