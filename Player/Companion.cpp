#include "Companion.hpp"
#include "Scene/PlayScene.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Point.hpp"
#include "Engine/Collider.hpp"
#include "Scene/player_data.h"
#include <allegro5/allegro_primitives.h>

using Res = Engine::Resources;

Companion::Companion(float x, float y)
    : Player("Characters/Support-1/image1x1.png", x, y, 100.0f, 100.0f, 190.0f), damage(1)
{
    std::string folder;
    if (std::string(companionType) == "COMP1")
        folder = "Support-1";
    else if (std::string(companionType) == "COMP2")
        folder = "Support-2";
    else
        folder = "Support-3";

    for (int i = 1; i <= 4; ++i)
        idleFrames.push_back(Res::GetInstance().GetBitmap("Characters/" + folder + "/image" + std::to_string(i) + "x1.png"));

    for (int i = 5; i <= 10; ++i)
        walkFrames.push_back(Res::GetInstance().GetBitmap("Characters/" + folder + "/image" + std::to_string(i) + "x1.png"));

    for (int i = 11; i <= 16; ++i)
        attackFrames.push_back(Res::GetInstance().GetBitmap("Characters/" + folder + "/image" + std::to_string(i) + "x1.png"));
    currentBitmap = idleFrames[0];
}

void Companion::Update(float dt)
{
    if (std::string(companionType) == "COMP1")
    {
        if (!wispSkillReady)
        {
            wispSkillTimer += dt;
            if (wispSkillTimer >= wispSkillCooldown)
            {
                wispSkillReady = true;
            }
        }
    }

    if (hp <= 0)
        return;

    // Movement
    Engine::Point v(0, 0);
    if (keyUp)
        v.y -= 1;
    if (keyDown)
        v.y += 1;
    if (keyLeft)
    {
        v.x -= 1;
        faceRight = false;
    }
    if (keyRight)
    {
        v.x += 1;
        faceRight = true;
    }

    movingFlag = (v.x || v.y);
    if (movingFlag)
    {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        v = v / len * GetSpeed() * dt;
        auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
        if (scene)
        {
            float nextX = Position.x + v.x;
            float nextY = Position.y + v.y;
            int gx = static_cast<int>(nextX) / PlayScene::BlockSize;
            int gy = static_cast<int>(nextY) / PlayScene::BlockSize;
            const float camX = scene->camera.x;
            const float camY = scene->camera.y;
            const float camW = Engine::GameEngine::GetInstance().GetScreenSize().x;
            const float camH = Engine::GameEngine::GetInstance().GetScreenSize().y;

            bool moved = false;

            // Try full diagonal move
            if (scene->IsWalkable(gx, gy) &&
                nextX >= camX && nextX <= camX + camW &&
                nextY >= camY && nextY <= camY + camH)
            {
                Position.x = nextX;
                Position.y = nextY;
                moved = true;
            }

            if (!moved)
            {
                // Try only X
                float testX = Position.x + v.x;
                int testGridX = static_cast<int>(testX / PlayScene::BlockSize);
                if (scene->IsWalkable(testGridX, static_cast<int>(Position.y / PlayScene::BlockSize)) &&
                    testX >= camX && testX <= camX + camW)
                {
                    Position.x = testX;
                    moved = true;
                }

                // Try only Y
                float testY = Position.y + v.y;
                int testGridY = static_cast<int>(testY / PlayScene::BlockSize);
                if (scene->IsWalkable(static_cast<int>(Position.x / PlayScene::BlockSize), testGridY) &&
                    testY >= camY && testY <= camY + camH)
                {
                    Position.y = testY;
                    moved = true;
                }
            }
        }
    }

    // Collision
    auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    bool attacked = false;
    if (scene)
    {
        for (auto *obj : scene->EnemyGroup->GetObjects())
        {
            Enemy *enemy = dynamic_cast<Enemy *>(obj);
            if (enemy &&
                !isRecharging &&
                attackQuota > 0 &&
                Engine::Collider::IsCircleOverlap(Position, GetRadius(), enemy->Position, enemy->GetRadius()))
            {

                enemy->Hit(damage);
                isAttacking = true;
                attackTimer = attackDuration;

                attackQuota -= quotaDepleteRate * dt;
                if (attackQuota <= 0)
                {
                    attackQuota = 0;
                    isRecharging = true;
                }

                timeSinceLastAttack = 0.0f;
                attacked = true;
                break;
            }
        }
    }

    if (!attacked && attackQuota < 100.0f && !isRecharging)
        timeSinceLastAttack += dt;

    if ((timeSinceLastAttack >= rechargeDelay && attackQuota < 100.0f) || isRecharging)
    {
        isRecharging = true;
        attackQuota += (100.0f / quotaRechargeTime) * dt;
        if (attackQuota >= 100.0f)
        {
            attackQuota = 100.0f;
            isRecharging = false;
            timeSinceLastAttack = 0.0f;
        }
    }

    // Animation update
    animTimer += dt;

    const auto &activeFrames =
        isAttacking ? attackFrames : (movingFlag ? walkFrames : idleFrames);

    if (animTimer >= animInterval && !activeFrames.empty())
    {
        animTimer = 0;
        currentFrame = (currentFrame + 1) % activeFrames.size();
        currentBitmap = activeFrames[currentFrame];
    }

    if (isAttacking)
    {
        attackTimer -= dt;
        if (attackTimer <= 0)
        {
            isAttacking = false;
            currentFrame = 0;
        }
    }

    Sprite::Update(dt);
}

Engine::Point Companion::PositionWeapon() const
{
    float offX = FacingRight() ? 2 : -2;
    return Engine::Point{Position.x + offX, Position.y + 7};
}

void Companion::OnKeyDown(int k)
{
    if (k == ALLEGRO_KEY_UP)
        keyUp = true;
    if (k == ALLEGRO_KEY_DOWN)
        keyDown = true;
    if (k == ALLEGRO_KEY_LEFT)
        keyLeft = true;
    if (k == ALLEGRO_KEY_RIGHT)
        keyRight = true;
    auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (k == ALLEGRO_KEY_P && scene)
    {
        for (auto *obj : scene->PlayerGroup->GetObjects())
        {
            if (Companion *sp = dynamic_cast<Companion *>(obj))
            {
                sp->TeleportToPlayer();
            }
        }
    }
    if (std::string(companionType) == "COMP1" && k == ALLEGRO_KEY_BACKSPACE && wispSkillReady)
    {
        PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
        if (scene)
        {
            Player *player = scene->GetPlayer();
            if (player)
            {
                // Swap positions
                Engine::Point temp = Position;
                Position = player->Position;
                player->Position = temp;

                // Set cooldown
                wispSkillReady = false;
                wispSkillTimer = 0.0f;
            }
        }
    }
}

void Companion::OnKeyUp(int k)
{
    if (k == ALLEGRO_KEY_UP)
        keyUp = false;
    if (k == ALLEGRO_KEY_DOWN)
        keyDown = false;
    if (k == ALLEGRO_KEY_LEFT)
        keyLeft = false;
    if (k == ALLEGRO_KEY_RIGHT)
        keyRight = false;
}

float Companion::GetRadius() const
{
    return 32.0f;
}

void Companion::TeleportToPlayer()
{
    auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    for (auto *obj : scene->PlayerGroup->GetObjects())
    {
        Player *mainPlayer = dynamic_cast<Player *>(obj);
        if (mainPlayer && mainPlayer != this)
        {
            // Offset to prevent overlap
            Position.x = mainPlayer->Position.x + 20;
            Position.y = mainPlayer->Position.y;
            return;
        }
    }
}

void Companion::Draw() const
{
    ALLEGRO_BITMAP *bmp = currentBitmap.get();
    float w = al_get_bitmap_width(bmp);
    float h = al_get_bitmap_height(bmp);
    float scaleX = faceRight ? 1.0f : -1.0f;
    float anchorX = faceRight ? Anchor.x : (1.0f - Anchor.x);

    ALLEGRO_COLOR tintColor = knockbackTime > 0 ? al_map_rgba_f(1.0f, 0.2f, 0.2f, 1.0f) : Tint;

    al_draw_tinted_scaled_rotated_bitmap(
        bmp, tintColor,
        anchorX * w, Anchor.y * h,
        Position.x, Position.y,
        scaleX * (Size.x / w),
        Size.y / h,
        Rotation, 0);

    if (PlayScene::DebugMode)
    {
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(180, 0, 255), 2);
    }

    // Cooldown/Quota bar
    float barWidth = 40;
    float barHeight = 5;
    float fillRatio = attackQuota / 100.0f;
    float barX = Position.x - barWidth / 2;
    float barY = Position.y - CollisionRadius - 20;

    // background (dark)
    al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(50, 50, 50));

    // fill (color depends on state)
    ALLEGRO_COLOR fillColor = isRecharging ? al_map_rgb(100, 100 + 155 * fillRatio, 255) : // blueish while recharging
                                  al_map_rgb(255 * (1 - fillRatio), 255 * fillRatio, 0);   // red→green while active

    al_draw_filled_rectangle(barX, barY, barX + barWidth * fillRatio, barY + barHeight, fillColor);

    // border
    al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(255, 255, 255), 1);
}
