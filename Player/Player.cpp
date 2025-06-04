#include "Player.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <allegro5/allegro_primitives.h>
#include <cmath>

Player::Player(float x, float y)
    : Engine::Sprite("play/player_1/image1x1.png", x, y),
      moveSpeed(200.0f), keyW(false), keyA(false), keyS(false), keyD(false),
      faceRight(true), animationTimer(0), currentFrame(0), animationInterval(0.1f)
{
    Anchor = Engine::Point(0.5f, 0.5f);

    for (int i = 1; i <= 7; ++i)
    {
        std::string path = "play/player_1/image" + std::to_string(i) + "x1.png";
        animationsRight.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }
    currentBitmap = animationsRight[0];
}

void Player::Update(float deltaTime)
{
    Engine::Point velocity(0, 0);
    if (keyW)
        velocity.y -= 1;
    if (keyS)
        velocity.y += 1;
    if (keyA)
    {
        velocity.x -= 1;
        faceRight = false;
    }
    if (keyD)
    {
        velocity.x += 1;
        faceRight = true;
    }

    bool moving = (velocity.x != 0 || velocity.y != 0);
    bool movingHorizontally = (velocity.x != 0);

    if (moving)
    {
        float len = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        Position.x += (velocity.x / len) * moveSpeed * deltaTime;
        Position.y += (velocity.y / len) * moveSpeed * deltaTime;
    }

    UpdateAnimation(deltaTime, movingHorizontally);
    Engine::Sprite::Update(deltaTime);
    float mapW = PlayScene::MapWidth * PlayScene::BlockSize;
    float mapH = PlayScene::MapHeight * PlayScene::BlockSize;
    float halfW = GetBitmapWidth() / 2.0f;
    float halfH = GetBitmapHeight() / 2.0f;

    Position.x = std::max(halfW, std::min(Position.x, mapW - halfW));
    Position.y = std::max(halfH, std::min(Position.y, mapH - halfH));
}

void Player::UpdateAnimation(float deltaTime, bool movingHorizontally)
{
    if (!movingHorizontally)
        return;
    animationTimer += deltaTime;
    if (animationTimer >= animationInterval)
    {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % animationsRight.size();
        currentBitmap = animationsRight[currentFrame];
    }
}

void Player::Draw() const
{
    if (!currentBitmap)
        return;

    ALLEGRO_BITMAP *bmp = currentBitmap.get();
    float bmpW = al_get_bitmap_width(bmp);
    float bmpH = al_get_bitmap_height(bmp);
    float scaleX = faceRight ? 1.0f : -1.0f;
    float anchorOffsetX = faceRight ? Anchor.x : (1.0f - Anchor.x);

    al_draw_tinted_scaled_rotated_bitmap(
        bmp,
        Tint,
        anchorOffsetX * bmpW,
        Anchor.y * bmpH,
        Position.x,
        Position.y,
        scaleX * (Size.x / bmpW),
        Size.y / bmpH,
        Rotation,
        0);
}

void Player::OnKeyDown(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_W)
        keyW = true;
    if (keyCode == ALLEGRO_KEY_A)
        keyA = true;
    if (keyCode == ALLEGRO_KEY_S)
        keyS = true;
    if (keyCode == ALLEGRO_KEY_D)
        keyD = true;
}

void Player::OnKeyUp(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_W)
        keyW = false;
    if (keyCode == ALLEGRO_KEY_A)
        keyA = false;
    if (keyCode == ALLEGRO_KEY_S)
        keyS = false;
    if (keyCode == ALLEGRO_KEY_D)
        keyD = false;
}
