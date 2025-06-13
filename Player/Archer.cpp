#include "Archer.hpp"
#include "Weapon/BowWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"

using Engine::Point;

Archer::Archer(float x, float y)
: Player("Characters/Archer/Idle/image1x1.png", x, y, 100.0f, 100.0f, 190.0f) {
    bow = new BowWeapon(PositionWeapon(), this);
    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(bow);

    CollisionRadius = 32;
    Size.x = 128;
    Size.y = 128;
    for (int i = 1; i <= 6; ++i)
    {
        std::string path = "Characters/Archer/Idle/image" + std::to_string(i) + "x1.png";
        idleFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Characters/Archer/Walk/image" + std::to_string(i) + "x1.png";
        walkFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 3; ++i)
    {
        std::string path = "Characters/Archer/Hurt/image" + std::to_string(i) + "x1.png";
        hurtFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 3; ++i)
    {
        std::string path = "Characters/Archer/Death/image" + std::to_string(i) + "x1.png";
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
