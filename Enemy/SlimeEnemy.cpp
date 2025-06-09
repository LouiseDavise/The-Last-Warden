#include "SlimeEnemy.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

SlimeEnemy::SlimeEnemy(float x, float y)
    : Enemy("Enemies/Slime/slime_walking/image1x1.png", x, y, 48, 40, 20, 1, 1),
      animationTimer(0), animationInterval(0.1f), currentFrame(0)
{
    Size.x = 48;
    Size.y = 48;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/Slime/slime_walking/image" + std::to_string(i) + "x1.png";
        animationFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    // Initial image
    bmp = animationFrames[0];
    Velocity = Engine::Point(0, 0);
}

void SlimeEnemy::Update(float deltaTime)
{
    // Update animation
    animationTimer += deltaTime;
    if (animationTimer >= animationInterval)
    {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % animationFrames.size();
        bmp = animationFrames[currentFrame];
    }

    // Move toward player
    auto *scene = getPlayScene();
    auto *target = scene->GetPlayer();

    if (target)
    {
        Engine::Point playerPos = target->Position;
        Engine::Point dir = playerPos - Position;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len != 0)
        {
            dir.x /= len;
            dir.y /= len;
            Velocity = dir * speed;
            float nextX = Position.x + Velocity.x * deltaTime;
            float nextY = Position.y + Velocity.y * deltaTime;

            int tileX = static_cast<int>(nextX) / PlayScene::BlockSize;
            int tileY = static_cast<int>(nextY) / PlayScene::BlockSize;

            if (tileX >= 0 && tileX < PlayScene::MapWidth &&
                tileY >= 0 && tileY < PlayScene::MapHeight &&
                scene->IsWalkable(tileX, tileY))
            {
                Position.x = nextX;
                Position.y = nextY;
            }
            else
            {
                Velocity = Engine::Point(0, 0); // Stop movement if not walkable
            }
        }
    }

    Enemy::Update(deltaTime);
}

void SlimeEnemy::UpdatePath(const std::vector<std::vector<int>> &mapDistance)
{
    int gx = static_cast<int>(Position.x) / PlayScene::BlockSize;
    int gy = static_cast<int>(Position.y) / PlayScene::BlockSize;

    if (gx < 0 || gx >= PlayScene::MapWidth || gy < 0 || gy >= PlayScene::MapHeight)
        return;
    if (mapDistance[gy][gx] == -1)
        return;

    Enemy::UpdatePath(mapDistance); // Only call if valid
}
