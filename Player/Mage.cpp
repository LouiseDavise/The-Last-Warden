#include "Mage.hpp"
#include "Weapon/WandWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"

using Engine::Point;

Mage::Mage(float x, float y)
    : Player("Characters/Mage/Idle/image1x1.png", x, y, 100.0f, 100.0f, 190.0f) {
    wand = new WandWeapon(PositionWeapon(), this);
    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(wand);

    CollisionRadius = 32;
    Size.x = 128;
    Size.y = 128;
    for (int i = 1; i <= 6; ++i)
    {
        std::string path = "Characters/Mage/Idle/image" + std::to_string(i) + "x1.png";
        idleFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 7; ++i)
    {
        std::string path = "Characters/Mage/Walk/image" + std::to_string(i) + "x1.png";
        walkFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 4; ++i)
    {
        std::string path = "Characters/Mage/Hurt/image" + std::to_string(i) + "x1.png";
        hurtFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 4; ++i)
    {
        std::string path = "Characters/Mage/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }
    state = PlayerState::Idle;
    walkTimer = 0;
    walkInterval = 0.12f;
    deathTimer = 0;
    deathInterval = 0.07f;
    idleTimer = 0;
    idleInterval = 0.12f;
    hurtTimer = 0;
    hurtInterval = 0.05f;
    currentFrame = 0;
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