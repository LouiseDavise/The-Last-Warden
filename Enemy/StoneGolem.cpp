#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "StoneGolem.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Collider.hpp"
#include "Scene/PlayScene.hpp"
#include "Structure/Structure.hpp"
#include "Structure/Offense/Tower.hpp"
#include "Projectile/Projectile.hpp"
#include <cmath>

int idleMarkRange = 9;
int idleMarkMelee = 7;
StoneGolem::StoneGolem(float x, float y)
    : Enemy("Enemies/StoneGolem/Run/image1x1.png", x, y, 300, 70, 2000, 2000, 1, 2.0f, 10)
{
    Size.x = 1000;
    Size.y = 1000;
    for (int i = 1; i <= 4; ++i)
    {
        std::string path = "Enemies/StoneGolem/Idle/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 4; ++i)
    {
        std::string path = "Enemies/StoneGolem/Idle/image" + std::to_string(i) + "x1.png";
        idleFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 12; ++i)
    {
        std::string path = "Enemies/StoneGolem/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 13; ++i)
    {
        std::string path;
        if(i <= idleMarkRange) path = "Enemies/StoneGolem/Range/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/StoneGolem/Idle/image" + std::to_string(i-idleMarkRange) + "x1.png";
        rangeFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    
    for (int i = 1; i <= 11; ++i)
    {
        std::string path;
        if(i <= idleMarkMelee) path = "Enemies/StoneGolem/Melee/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/StoneGolem/Idle/image" + std::to_string(i-idleMarkMelee) + "x1.png";
        meleeFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }


    // Initial image
    bmp = runFrames[0];
    Velocity = Engine::Point(0, 0);
    state = State::Run;
    runTimer = 0;
    runInterval = 0.12f;
    deathTimer = 0;
    deathInterval = 0.12f;
    meleeTimer = 0;
    meleeInterval = 0.055f;
    rangeTimer = 0;
    rangeInterval = 0.055f;
    currentFrame = 0;
}

void StoneGolem::Hit(float damage)
{
    if (state == State::Dying)
        return;
    Engine::LOG(Engine::INFO) << "BOSS got hit: " << damage;

    hp -= damage;
    if (hp <= 0)
    {
        state = State::Dying;
        currentFrame = 0;
        runTimer = 0;
        deathTimer = 0;
        attackTimer = 0;
        hurtTimer = 0;
        Velocity = Engine::Point(0, 0); // Stop movement
        for (auto &it : lockedTowers)
            it->Target = nullptr;
        for (auto &it : lockedProjectiles)
            it->Target = nullptr;
        getPlayScene()->AddMoney(money);
        getPlayScene()->IncreaseKillCount();
    }
    else if(state != State::Hurt && state != State::Charging && state != State::Blinking)
    {
        wasRunning = (state == State::Run);
        currentFrame = 0;GO
    }
}

void StoneGolem::Update(float deltaTime)
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

    else if (state == State::BOSS_Range) {
        rangeTimer += deltaTime;
        cooldownTimer += deltaTime;
        if(rangeTimer >= rangeInterval){
            rangeTimer = 0;
            if(currentFrame < idleMarkRange - 1)currentFrame++;
            if(currentFrame >= idleMarkRange - 1){
                if(!playerCollide && !collidedStructure){
                    state = State::Run;
                    currentFrame = 0;
                }
                currentFrame++;
                if (currentFrame >= rangeFrames.size())
                    currentFrame = idleMarkRange;
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

    else if (state == State::BOSS_Melee) {
        meleeTimer += deltaTime;
        cooldownTimer += deltaTime;
        if(meleeTimer >= meleeInterval){
            meleeTimer = 0;
            if(currentFrame < idleMarkMelee - 1)currentFrame++;
            if(currentFrame >= idleMarkMelee - 1){
                if(!playerCollide && !collidedStructure){
                    state = State::Run;
                    currentFrame = 0;
                }
                currentFrame++;
                if (currentFrame >= meleeFrames.size())
                    currentFrame = idleMarkMelee;
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

    if (playerCollide)
    {
        if (state != State::BOSS_Melee)
        {
            state = State::BOSS_Melee;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0); // Stop movement
        }
        player->Hit(GetDamage(), Position);
    }
    else if (collidedStructure)
    {
        if (state != State::BOSS_Melee)
        {
            state = State::BOSS_Melee;
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
        faceRight = Velocity.x <= 0;
    }
}

