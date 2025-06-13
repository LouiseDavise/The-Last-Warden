#include "Player.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/LOG.hpp"
#include <allegro5/allegro_primitives.h>
#include <cmath>

using Engine::Point;
using Res = Engine::Resources;

Player::Player(std::string img, float x, float y, float hp, float MAXhp, float moveSpeed)
    : Sprite(img, x, y), hp(hp), MAXhp(MAXhp), moveSpeed(moveSpeed)
{
    Anchor.x = 0.5f;
    Anchor.y = 0.75f;
}

void Player::Update(float dt)
{
    if (hp > 0)UpdateMovement(dt);

    if (hp <= 0) {
        if(state != PlayerState::Death){
            state = PlayerState::Death;
            currentFrame = 0;
        }
    }
    else if (knockbackTime > 0) {
        if(state != PlayerState::Hurt){
            state = PlayerState::Hurt;
            currentFrame = 0;
        }
    }
    else if (movingFlag) {
        if(state != PlayerState::Walk) currentFrame = 0;
        state = PlayerState::Walk;
    }
    else {
        if(state != PlayerState::Idle) currentFrame = 0;
        state = PlayerState::Idle;
    }

    UpdateAnimation(dt);
    Sprite::Update(dt);

    float mapW = PlayScene::MapWidth * PlayScene::BlockSize;
    float mapH = PlayScene::MapHeight * PlayScene::BlockSize;
    float halfW = GetBitmapWidth() / 2.0f;
    float halfH = GetBitmapHeight() / 2.0f;
    Position.x = std::clamp(Position.x, halfW, mapW - halfW + 8);
    Position.y = std::clamp(Position.y, halfH, mapH - halfH - 8);
}

void Player::UpdateMovement(float dt)
{
    if (hp <= 0)
        return;

    if (knockbackTime > 0)
    {
        float nextX = Position.x + knockbackVelocity.x * dt;
        float nextY = Position.y + knockbackVelocity.y * dt;

        int gridX = static_cast<int>(nextX / PlayScene::BlockSize);
        int gridY = static_cast<int>(nextY / PlayScene::BlockSize);

        PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
        if (scene && scene->IsWalkable(gridX, gridY))
        {
            Position.x = nextX;
            Position.y = nextY;
        }

        knockbackTime -= dt;
        knockbackVelocity.x *= 0.9f;
        knockbackVelocity.y *= 0.9f;
        return;
    }

    Point v(0, 0);
    if (keyW)
        v.y -= 1;
    if (keyS)
        v.y += 1;
    if (keyA)
    {
        v.x -= 1;
        faceRight = false;
    }
    if (keyD)
    {
        v.x += 1;
        faceRight = true;
    }

    movingFlag = (v.x || v.y);
    if (!movingFlag)
        return;

    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0)
        return;

    v = v / len * moveSpeed * dt;

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene)
        return;

    float nextX = Position.x + v.x;
    float nextY = Position.y + v.y;
    int gridX = static_cast<int>(nextX / PlayScene::BlockSize);
    int gridY = static_cast<int>(nextY / PlayScene::BlockSize);

    if (scene->IsWalkable(gridX, gridY))
    {
        Position.x = nextX;
        Position.y = nextY;
    }
    else
    {
        bool moved = false;

        // Try X only
        float testX = Position.x + v.x;
        int testGridX = static_cast<int>(testX / PlayScene::BlockSize);
        if (scene->IsWalkable(testGridX, static_cast<int>(Position.y / PlayScene::BlockSize)))
        {
            Position.x = testX;
            moved = true;
        }

        // Try Y only
        float testY = Position.y + v.y;
        int testGridY = static_cast<int>(testY / PlayScene::BlockSize);
        if (scene->IsWalkable(static_cast<int>(Position.x / PlayScene::BlockSize), testGridY))
        {
            Position.y = testY;
            moved = true;
        }

        if (!moved)
        {
            // stuck
        }
    }
}

void Player::UpdateAnimation(float dt) {
    switch (state) {
        case PlayerState::Walk:
            walkTimer += dt;
            if (walkTimer >= walkInterval && walkFrames.size() > 0) {
                walkTimer = 0;
                currentFrame = (currentFrame + 1) % walkFrames.size();
            }
            break;

        case PlayerState::Idle:
            idleTimer += dt;
            if (idleTimer >= idleInterval && idleFrames.size() > 0) {
                idleTimer = 0;
                currentFrame = (currentFrame + 1) % idleFrames.size();
            }
            break;

        case PlayerState::Hurt:
            hurtTimer += dt;
            if (hurtTimer >= hurtInterval && hurtFrames.size() > 0) {
                hurtTimer = 0;
                currentFrame++;
                if (currentFrame >= hurtFrames.size()) {
                    state = movingFlag ? PlayerState::Walk : PlayerState::Idle;
                    currentFrame = 0;
                }
            }
            break;

        case PlayerState::Death:
            deathTimer += dt;
            if (deathTimer >= deathInterval && deathFrames.size() > 0) {
                deathTimer = 0;
                currentFrame++;
                if (currentFrame >= deathFrames.size()) {
                    currentFrame = deathFrames.size() - 1;
                    isDead = true;
                    return;
                }
            }
            break;
    }
}


void Player::Draw() const
{
    ALLEGRO_BITMAP* bmp = [&]() {
        switch (state) {
            case PlayerState::Walk:
                return walkFrames[currentFrame].get();
            case PlayerState::Idle:
                return idleFrames[currentFrame].get();
            case PlayerState::Hurt:
                return hurtFrames[currentFrame].get();
            case PlayerState::Death:
                return deathFrames[currentFrame].get();
        }
        return walkFrames[0].get(); // fallback
    }();
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

    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(180, 0, 255), 2);
    }
}

void Player::OnKeyDown(int k)
{
    if (k == ALLEGRO_KEY_W)
        keyW = true;
    if (k == ALLEGRO_KEY_A)
        keyA = true;
    if (k == ALLEGRO_KEY_S)
        keyS = true;
    if (k == ALLEGRO_KEY_D)
        keyD = true;
}
void Player::OnKeyUp(int k)
{
    if (k == ALLEGRO_KEY_W)
        keyW = false;
    if (k == ALLEGRO_KEY_A)
        keyA = false;
    if (k == ALLEGRO_KEY_S)
        keyS = false;
    if (k == ALLEGRO_KEY_D)
        keyD = false;
}

void Player::Hit(float dmg, const Engine::Point &from)
{
    hp -= dmg;
    if (hp < 0)
        hp = 0;

    Engine::Point dir = Position - from;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0)
    {
        dir.x /= len;
        dir.y /= len;

        float force = 600; // adjust strength
        knockbackVelocity = Engine::Point(dir.x * force, dir.y * force);
        knockbackTime = 0.2f; // 0.2 seconds of knockback
    }
}

void Player::OnMouseDown(int button, int mx, int my)
{
}