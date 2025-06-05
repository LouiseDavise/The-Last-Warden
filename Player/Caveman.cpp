#include "Caveman.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"

using Engine::Point;

Caveman::Caveman(float x, float y)
    : Player(x, y, 100.0f, 190.0f, "play/player_1/image1x1.png", 7, 10.0f)
{
    spear = std::make_unique<SpearWeapon>(PositionWeapon(), this);

    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(spear.get());
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
}

Point Caveman::PositionWeapon() const
{
    float offX = FacingRight() ? 2 : -2;
    return Point{ Position.x + offX, Position.y + 7};
}
