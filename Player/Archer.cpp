#include "Archer.hpp"
#include "Weapon/BowWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"

using Engine::Point;

Archer::Archer(float x, float y)
    : Player(x, y, 100.0f, 190.0f, "Characters/Archer/image1x1.png", "image", 7, 10.0f) {
    bow = new BowWeapon(PositionWeapon(), this);
    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(bow);

    CollisionRadius = 32;
}

void Archer::Update(float dt) {
    Player::Update(dt);
    bow->Update(dt);
}

void Archer::Draw() const {
    Player::Draw();
}

void Archer::OnMouseDown(int button, int mx, int my) {
    if (!(button & 1)) return;

    auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    float worldX = scene->camera.x + mx;
    float worldY = scene->camera.y + my;

    bow->Use(worldX, worldY);
    bow->Position = PositionWeapon();
}

Point Archer::PositionWeapon() const {
    float offX = FacingRight() ? 2 : -2;
    return Point{Position.x + offX, Position.y + 7};
}
