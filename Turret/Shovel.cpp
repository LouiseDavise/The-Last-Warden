#include "Turret/Shovel.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "Engine/GameEngine.hpp"
#include <iostream>

const int Shovel::Cost = 0;

Shovel::Shovel(float x, float y)
    : Sprite("play/shovel.png", x, y, 0, 0, 0, 0)
{
    Anchor = Engine::Point(0.5, 0.5);
}

void Shovel::Update(float deltaTime)
{
    Sprite::Update(deltaTime);
}

void Shovel::OnClick(int gridX, int gridY)
{
    auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    auto &&objects = scene->TowerGroup->GetObjects();
    for (auto *obj : objects)
    {
        Turret *turret = dynamic_cast<Turret *>(obj);
        if (!turret)
            continue;

        int tx = turret->Position.x / PlayScene::BlockSize;
        int ty = turret->Position.y / PlayScene::BlockSize;

        if (tx == gridX && ty == gridY)
        {
            scene->EarnMoney(turret->GetPrice() / 2);
            scene->TowerGroup->RemoveObject(turret->GetObjectIterator());
            turret->Visible = false;
            scene->mapState[ty][tx] = PlayScene::GetFloorTile();
            break;
        }
    }
}
