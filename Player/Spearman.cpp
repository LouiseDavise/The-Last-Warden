#include "Spearman.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/AudioHelper.hpp"

using Engine::Point;

// x, y, maxhp, speed, path, frames, animfps
Spearman::Spearman(float x, float y)
    : Player("Characters/Spearman/Idle/image1x1.png", x, y, 100.0f, 100.0f, 190.0f) {
    spear = new SpearWeapon(PositionWeapon(), this);
    if (auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene()))
        scene->WeaponGroup->AddNewObject(spear); // WeaponGroup takes ownership

    CollisionRadius = 32;
    Size.x = 128;
    Size.y = 128;
    for (int i = 1; i <= 8; ++i) {
        std::string path = "Characters/Spearman/Idle/image" + std::to_string(i) + "x1.png";
        idleFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 8; ++i) {
        std::string path = "Characters/Spearman/Walk/image" + std::to_string(i) + "x1.png";
        walkFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 3; ++i) {
        std::string path = "Characters/Spearman/Hurt/image" + std::to_string(i) + "x1.png";
        hurtFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 9; ++i) {
        std::string path = "Characters/Spearman/Death/image" + std::to_string(i) + "x1.png";
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

    spearLoopAudio = nullptr;
    wasReturning = false;
}

void Spearman::Update(float dt) {
    Player::Update(dt);
    spear->Update(dt);

    bool isNowReturning = spear->IsReturning();

    // Just started returning
    if (isNowReturning && !wasReturning) {
        spearLoopAudio = AudioHelper::PlaySample("spearman-retract.mp3", true, AudioHelper::SFXVolume);
    }

    // Just finished returning
    if (!isNowReturning && wasReturning) {
        if (spearLoopAudio) {
            AudioHelper::StopSample(spearLoopAudio);
            spearLoopAudio = nullptr;
        }
    }

    wasReturning = isNowReturning;
}

void Spearman::Draw() const {
    Player::Draw(); // drawn by WeaponGroup
}

void Spearman::OnMouseDown(int button, int mx, int my) {
    if (!(button & 1))
        return; // left mouse only

    auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    float worldX = scene->camera.x + mx;
    float worldY = scene->camera.y + my;

    // Play throw sound if spear is not already active
    if (!spear->IsFlying() && !spear->IsSpinningBeforeReturn() && !spear->IsReturning()) {
        AudioHelper::PlayAudio("spearman-throw.mp3");
    }

    spear->Use(worldX, worldY);
}

Point Spearman::PositionWeapon() const {
    float offX = FacingRight() ? 2 : -2;
    return Point{Position.x + offX, Position.y + 7};
}
