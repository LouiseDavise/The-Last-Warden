#include "Weapon.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"

Weapon::Weapon(const std::string &bmpPath,
               float dmg,
               float projectileSpeed,
               const Engine::Point &startPos,
               Player *owner)
    : Engine::Sprite(bmpPath, startPos.x, startPos.y),
      damage(dmg),
      speed(projectileSpeed),
      owner(owner)
{
    Anchor = Engine::Point(0.5f, 0.5f);
}

void Weapon::Update(float dt)
{
    if (available && owner)
    {
        Position = owner->PositionWeapon();

        // Rotate toward mouse
        auto scene = GetPlayScene();
        if (scene)
        {
            Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();
            Engine::Point worldMouse = mouse + scene->camera;
            Engine::Point dir = worldMouse - Position;
            if (dir.Magnitude() >= 1e-3f)
                Rotation = std::atan2(dir.y, dir.x);
        }
    }
    Engine::Sprite::Update(dt); // nothing else by default
}

void Weapon::Draw() const
{
    Engine::Sprite::Draw();
}

void Weapon::Reclaim()
{
    if (!owner)
        return;
    available = true;
    Position = owner->Position;
}

PlayScene *Weapon::GetPlayScene() const
{
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
