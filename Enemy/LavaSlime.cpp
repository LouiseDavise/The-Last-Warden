#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "LavaSlime.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Collider.hpp"
#include "Structure/Structure.hpp"
#include "Structure/Offense/Tower.hpp"
#include "UI/Animation/AreaEffect.hpp"

#include <cmath>

LavaSlime::LavaSlime(float x, float y)
    : Enemy("Enemies/LavaSlime/Run/image1x1.png", x, y, 32, 130, 50, 50, 20, 1.0f, 30)
{
    Size.x = 128;
    Size.y = 128;
    idleMark = 7;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/LavaSlime/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 10; ++i)
    {
        std::string path = "Enemies/LavaSlime/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 13; ++i)
    {
        std::string path;
        if(i <= idleMark) path = "Enemies/LavaSlime/Attack/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/LavaSlime/Idle/image" + std::to_string(i-idleMark) + "x1.png";
        attackFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 5; ++i)
    {
        std::string path = "Enemies/LavaSlime/Hurt/image" + std::to_string(i) + "x1.png";
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
    attackInterval = 0.07f;
    hurtTimer = 0;
    hurtInterval = 0.05f;
    currentFrame = 0;
}

void LavaSlime::Update(float deltaTime)
{
    auto* scene = getPlayScene();
    auto* player = scene->GetPlayer();

    bool playerCollide = player && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, player->Position, player->CollisionRadius) && player->GetHP() > 0 && hp > 0;

    Structure* collidedStructure = nullptr;
    for (auto* obj : scene->StructureGroup->GetObjects()) {
        Structure* structure = dynamic_cast<Structure*>(obj);
        if (structure && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, structure->Position, structure->CollisionRadius)) {
            collidedStructure = structure;
            break;
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
                if(!playerCollide && !collidedStructure){
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

            if (playerCollide) {
                player->Hit(GetDamage(), Position);
            } else if (collidedStructure) {
                collidedStructure->Hit(GetDamage());
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

    if (playerCollide)
    {
        if (state != State::Attacking)
        {
            state = State::Attacking;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0); // Stop movement
        }
        player->Hit(GetDamage(), Position);
    }
    else if (collidedStructure)
    {
        if (state != State::Attacking)
        {
            state = State::Attacking;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0); // Stop movement
        }
        collidedStructure->Hit(GetDamage());
    }
    else if (scene->validLine(Position, player->Position))
    {
        state = State::Run;
        Engine::Point dir = (player->Position - Position).Normalize();
        Velocity = dir * speed;
    }
    else
    {
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

void LavaSlime::AOEAttack() {
    auto* scene = getPlayScene();
    const float aoeRadius = 96.0f;

    for (auto* obj : scene->StructureGroup->GetObjects()) {
        Structure* s = dynamic_cast<Structure*>(obj);
        if (!s) continue;
        if (Engine::Collider::IsCircleOverlap(Position, aoeRadius, s->Position, s->CollisionRadius)) {
            s->Hit(GetDamage());
        }
    }

    // Optionally: also damage the player
    auto* player = scene->GetPlayer();
    if (player && Engine::Collider::IsCircleOverlap(Position, aoeRadius, player->Position, player->CollisionRadius)) {
        player->Hit(GetDamage(), Position);
    }

    // Optional visual effect:
    scene->EffectGroup->AddNewObject(new AreaEffect(Position.x, Position.y, 96.0f, 0.5f, al_map_rgb(255, 0, 0)));
}