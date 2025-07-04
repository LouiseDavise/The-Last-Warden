#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Vampire.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Collider.hpp"
#include "Structure/Structure.hpp"
#include <cmath>

Vampire::Vampire(float x, float y)
    : Enemy("Enemies/Vampire/Run/image1x1.png", x, y, 48, 175, 700, 700, 20, 0.6f, 35)
{
    Size.x = 192;
    Size.y = 192;
    idleMark = 12;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/Vampire/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 11; ++i)
    {
        std::string path = "Enemies/Vampire/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 16; ++i)
    {
        std::string path;
        if(i <= idleMark) path = "Enemies/Vampire/Attack/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/Vampire/Idle/image" + std::to_string(i-idleMark) + "x1.png";
        attackFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 4; ++i)
    {
        std::string path = "Enemies/Vampire/Hurt/image" + std::to_string(i) + "x1.png";
        hurtFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }


    // Initial image
    bmp = runFrames[0];
    Velocity = Engine::Point(0, 0);
    state = State::Run;
    runTimer = 0;
    runInterval = 0.12f;
    deathTimer = 0;
    deathInterval = 0.12f;
    attackTimer = 0;
    attackInterval = 0.12f;
    hurtTimer = 0;
    hurtInterval = 0.05f;
    currentFrame = 0;
}

void Vampire::Update(float deltaTime)
{
    auto* scene = getPlayScene();
    auto* player = scene->GetPlayer();

    bool hasTargetCollide = false;
    Engine::Point damageTargetPos;

    if (player && player->GetHP() > 0 && Engine::Collider::IsCircleOverlap(Position, attackRange, player->Position, player->CollisionRadius)) {
        hasTargetCollide = true;
        damageTargetPos = player->Position;
    }

    if (!hasTargetCollide) {
        for (auto& it : scene->StructureGroup->GetObjects()) {
            Structure* s = dynamic_cast<Structure*>(it);
            if (!s || s->GetHP() <= 0) continue;
            if (Engine::Collider::IsCircleOverlap(Position, attackRange, s->Position, s->CollisionRadius)) {
                hasTargetCollide = true;
                damageTargetPos = s->Position;
                break;
            }
        }
    }

    if (state == State::Run)
    {
        runTimer += deltaTime;
        if (runTimer >= runInterval)
        {
            runTimer = 0;
            currentFrame = (currentFrame + 1) % runFrames.size();
            bmp = runFrames[currentFrame];
        }
    }
    else if (state == State::Dying)
    {
        if (deathFrames.empty())
        {
            getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
            return;
        }
        deathTimer += deltaTime;
        if (deathTimer >= deathInterval && deathFrames.size() > 0)
        {
            deathTimer = 0;
            currentFrame++;
            if (currentFrame >= deathFrames.size())
            {
                getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
                currentFrame = 0;
                return;
            }
        }
        bmp = deathFrames[currentFrame]; // Set death frame
        return;
    }

    else if (state == State::Attacking) {
        attackTimer += deltaTime;
        cooldownTimer += deltaTime;
        if(attackTimer >= attackInterval){
            attackTimer = 0;
            if(currentFrame < idleMark - 1)currentFrame++;
            if(currentFrame >= idleMark - 1){
                if(!hasTargetCollide){
                    state = State::Run;
                    currentFrame = 0;
                }
                currentFrame++;
                if (currentFrame >= attackFrames.size())
                    currentFrame = idleMark;
            }

        }
        
        if (cooldownTimer >= atkcd) {
            cooldownTimer = 0;
            currentFrame = 0;

            if (hasTargetCollide) {
                Engine::Point dir = (damageTargetPos - Position).Normalize();
                float bulletSpeed = 400.0f; // example speed
                scene->ProjectileGroup->AddNewObject(new EnemyCircular(Position.x, Position.y, dir.x * bulletSpeed, dir.y * bulletSpeed));
            }else{
                state = State::Run;
            }
        }

        return;
    }

    else if (state == State::Hurt)
    {
        hurtTimer += deltaTime;
        if (hurtTimer >= hurtInterval)
        {
            hurtTimer = 0;
            currentFrame++;
            if (currentFrame >= hurtFrames.size())
            {
                state = State::Run;
                currentFrame = 0;
                return;
            }
        }
        bmp = hurtFrames[currentFrame];
        auto *scene = getPlayScene();
        auto *player = scene->GetPlayer();
        if (player)
        {
            Engine::Point dir = (player->Position - Position).Normalize();
            Engine::Point slowVel = dir * speed * 0.675f;
            Position.x += slowVel.x * deltaTime;
            Position.y += slowVel.y * deltaTime;
        }
        return;
    }

    if (hasTargetCollide)
    {
        if (state != State::Attacking)
        {
            state = State::Attacking;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0); // Stop movement
        }
        Engine::Point dir = (damageTargetPos - Position).Normalize();
        float bulletSpeed = 400.0f; // example speed
        scene->ProjectileGroup->AddNewObject(new EnemyCircular(Position.x, Position.y, dir.x * bulletSpeed, dir.y * bulletSpeed));
    }
    else{
        state = State::Run;
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

    if (Velocity.x != 0)
    {
        faceRight = Velocity.x > 0;
    }
}