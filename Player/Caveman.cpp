#include "Caveman.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"

using Engine::Point;

// x, y, maxhp, speed, path, frames, animfps
Caveman::Caveman(float x, float y)
    : Player(x, y, 100.0f, 190.0f, "Characters/Mage1x1.png", "Mage", 7, 10.0f)
{
    spear = new SpearWeapon(PositionWeapon(), this);
    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(spear); // WeaponGroup takes ownership

    CollisionRadius = 32;
}

void Caveman::Update(float dt)
{
    Player::Update(dt);
    spear->Update(dt);
}

void Caveman::Draw() const
{
    Player::Draw();          // drawn by WeaponGroup
}

void Caveman::OnMouseDown(int button, int mx, int my)
{
    if (!(button & 1)) return;   // left mouse only
    auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    float worldX = scene->camera.x + mx;
    float worldY = scene->camera.y + my;

    spear->Use(worldX, worldY);
    spear->Position.x = PositionWeapon().x;
    spear->Position.y = PositionWeapon().y;
}

Point Caveman::PositionWeapon() const
{
    float offX = FacingRight() ? 2 : -2;
    return Point{ Position.x + offX, Position.y + 7};
}
