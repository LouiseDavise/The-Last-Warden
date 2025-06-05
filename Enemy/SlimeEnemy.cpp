#include "SlimeEnemy.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

SlimeEnemy::SlimeEnemy(float x, float y, float speed)
    : Enemy("play/enemy/slime_walking/image1x1.png", x, y, 0, 0, 1, 0),
      animationTimer(0), animationInterval(0.1f), currentFrame(0)
{
    Size.x = 48;
    Size.y = 48;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "play/enemy/slime_walking/image" + std::to_string(i) + "x1.png";
        animationFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    // Initial image
    bmp = animationFrames[0];
    Velocity = Engine::Point(0, 0);
    this->speed = speed;
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
    Engine::Point playerPos = scene->player->Position;
    Engine::Point dir = playerPos - Position;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0)
    {
        dir.x /= len;
        dir.y /= len;
        Velocity = dir * speed;
        Position.x += Velocity.x * deltaTime;
        Position.y += Velocity.y * deltaTime;
    }

    Enemy::Update(deltaTime);
}

void SlimeEnemy::UpdatePath(const std::vector<std::vector<int>> &mapDistance)
{
    int gx = static_cast<int>(Position.x) / PlayScene::BlockSize;
    int gy = static_cast<int>(Position.y) / PlayScene::BlockSize;

    if (gx < 0 || gx >= PlayScene::MapWidth || gy < 0 || gy >= PlayScene::MapHeight)
    {
        return;
    }

    if (mapDistance[gy][gx] == -1)
    {
        return;
    }

    Enemy::UpdatePath(mapDistance); // Only call if valid
}
