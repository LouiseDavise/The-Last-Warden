#include "Bom.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"

const int Bom::Cost = 40;

Bom::Bom(float x, float y)
    : Sprite("play/landmine.png", x, y, 0, 0, 0, 0)
{
    Anchor = Engine::Point(0.5, 0.5);
}

void Bom::Update(float deltaTime)
{
    Sprite::Update(deltaTime);
    if (!placed || activated)
        return;

    auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());

    for (auto &obj : scene->EnemyGroup->GetObjects())
    {
        Enemy *enemy = dynamic_cast<Enemy *>(obj);
        if (enemy && Engine::Collider::IsCircleOverlap(Position, 10, enemy->Position, enemy->CollisionRadius))
        {
            enemy->Hit(40);
            scene->EffectGroup->AddNewObject(new Engine::Sprite("play/explosion-4.png", Position.x, Position.y));
            activated = true;
            break;
        }
    }

    if (activated)
    {
        scene->RemovePreviewIfMatched(this);
        scene->GroundEffectGroup->RemoveObject(GetObjectIterator());
    }
}
