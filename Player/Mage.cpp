#include "Mage.hpp"
#include "Weapon/WandWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"

using Engine::Point;

Mage::Mage(float x, float y)
    : Player(x, y, 100.0f, 190.0f, "Characters/Mage/image1x1.png", "image", 7, 10.0f) {
    wand = new WandWeapon(PositionWeapon(), this);
    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(wand);

    CollisionRadius = 32;
}

void Mage::Update(float dt) {
    Player::Update(dt);
    wand->Update(dt);
}

void Mage::Draw() const {
    Player::Draw();
}

void Mage::OnMouseDown(int button, int mx, int my) {
    if (!(button & 1))
        return;

    auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    float worldX = scene->camera.x + mx;
    float worldY = scene->camera.y + my;

    wand->Use(worldX, worldY);
    wand->Position = PositionWeapon();
}

Point Mage::PositionWeapon() const {
    float offX = FacingRight() ? 2 : -2;
    return Point{Position.x + offX, Position.y + 7};
}