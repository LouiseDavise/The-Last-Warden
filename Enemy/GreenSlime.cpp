#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "GreenSlime.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

GreenSlime::GreenSlime(float x, float y)
    : Enemy("Enemies/GreenSlime/Run/image1x1.png", x, y, 48, 40, 70, 1, 1),
      animationTimer(0), animationInterval(0.12f), currentFrame(0)
{
    Size.x = 128;
    Size.y = 64;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/GreenSlime/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    // Initial image
    bmp = runFrames[0];
    Velocity = Engine::Point(0, 0);
    faceRight = false;
}

void GreenSlime::Update(float deltaTime)
{
    animationTimer += deltaTime;
    if (animationTimer >= animationInterval)
    {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % runFrames.size();
        bmp = runFrames[currentFrame];
    }

    auto *scene = getPlayScene();
    if (!scene || !scene->GetPlayer())
        return;

    Engine::Point playerPos = scene->GetPlayer()->Position;
    float distToPlayer = (playerPos - Position).Magnitude();

    if (distToPlayer <= PlayScene::BlockSize * 1.2f || scene->validLine(Position, playerPos))
    {
        Engine::Point dir = (playerPos - Position).Normalize();
        Velocity = dir * speed;
    }
    else
    {
        int gx = static_cast<int>(Position.x) / PlayScene::BlockSize;
        int gy = static_cast<int>(Position.y) / PlayScene::BlockSize;

        if (gx < 0 || gx >= PlayScene::MapWidth || gy < 0 || gy >= PlayScene::MapHeight)
            return;

        int currDist = scene->mapDistance[gy][gx];
        if (currDist <= 0)
            return;

        Engine::Point avgDir(0, 0);
        int count = 0;

        for (const auto &dir : PlayScene::directions)
        {
            int nx = gx + dir.x;
            int ny = gy + dir.y;
            if (nx < 0 || nx >= PlayScene::MapWidth || ny < 0 || ny >= PlayScene::MapHeight)
                continue;
            if (!scene->IsWalkable(nx, ny))
                continue;

            int neighborDist = scene->mapDistance[ny][nx];
            if (neighborDist >= 0 && neighborDist < currDist)
            {
                avgDir.x += dir.x;
                avgDir.y += dir.y;
                count++;
            }
        }

        if (count > 0)
        {
            avgDir.x /= count;
            avgDir.y /= count;

            float len = std::sqrt(avgDir.x * avgDir.x + avgDir.y * avgDir.y);
            if (len > 0)
            {
                Engine::Point norm = avgDir / len;
                Velocity = norm * speed;
            }
            else
            {
                Velocity = Engine::Point(0, 0);
            }
        }
    }

    // Movement with slide-along-wall fallback
    float nextX = Position.x + Velocity.x * deltaTime;
    float nextY = Position.y + Velocity.y * deltaTime;

    int tileX = static_cast<int>(nextX) / PlayScene::BlockSize;
    int tileY = static_cast<int>(nextY) / PlayScene::BlockSize;

    bool moved = false;

    if (scene->IsWalkable(tileX, tileY))
    {
        Position.x = nextX;
        Position.y = nextY;
        moved = true;
    }
    else
    {
        // Try moving only X
        float testX = Position.x + Velocity.x * deltaTime;
        int testTileX = static_cast<int>(testX) / PlayScene::BlockSize;
        if (scene->IsWalkable(testTileX, static_cast<int>(Position.y) / PlayScene::BlockSize))
        {
            Position.x = testX;
            moved = true;
        }

        // Try moving only Y
        float testY = Position.y + Velocity.y * deltaTime;
        int testTileY = static_cast<int>(testY) / PlayScene::BlockSize;
        if (scene->IsWalkable(static_cast<int>(Position.x) / PlayScene::BlockSize, testTileY))
        {
            Position.y = testY;
            moved = true;
        }
    }

    if (!moved)
        Velocity = Engine::Point(0, 0);

    if (Velocity.x != 0) {
        faceRight = Velocity.x > 0;
    }
}


void GreenSlime::UpdatePath(const std::vector<std::vector<int>> &mapDistance)
{
    int gx = static_cast<int>(Position.x) / PlayScene::BlockSize;
    int gy = static_cast<int>(Position.y) / PlayScene::BlockSize;

    if (gx < 0 || gx >= PlayScene::MapWidth || gy < 0 || gy >= PlayScene::MapHeight)
        return;
    if (mapDistance[gy][gx] == -1)
        return;
}

void GreenSlime::Draw() const {
    ALLEGRO_BITMAP* frame = runFrames[currentFrame].get();
    float cx = Anchor.x * al_get_bitmap_width(frame);
    float cy = Anchor.y * al_get_bitmap_height(frame);
    float scaleX = Size.x / al_get_bitmap_width(frame);
    float scaleY = Size.y / al_get_bitmap_height(frame);
    int flags = faceRight ? ALLEGRO_FLIP_HORIZONTAL : 0;

    al_draw_tinted_scaled_rotated_bitmap(frame, Tint, cx, cy, Position.x, Position.y, scaleX, scaleY, Rotation, flags);
    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    }
}

