#include "Player.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <allegro5/allegro_primitives.h>
#include <cmath>

using Engine::Point;
using Res = Engine::Resources;

Player::Player(float x, float y,float hp,float moveSpeed, const std::string& firstFramePath, const std::string& namePrefix, int frames, float animFPS)
    : Sprite(firstFramePath, x, y),
      hp(hp), moveSpeed(moveSpeed)
{
    Anchor = Point(0.5f, 0.5f);
    animInterval = 1.0f / animFPS;
    LoadAnimation(firstFramePath.substr(0,
                  firstFramePath.find_last_of('/') + 1) + namePrefix, frames);
    currentBitmap = framesRight[0];
}

void Player::LoadAnimation(const std::string& prefix, int frames)
{
    framesRight.clear();
    for (int i = 1; i <= frames; ++i) {
        std::string path = prefix + std::to_string(i) + "x1.png";
        framesRight.push_back(Res::GetInstance().GetBitmap(path));
    }
}

void Player::Update(float dt)
{
    UpdateMovement(dt);
    UpdateAnimation(dt);
    Sprite::Update(dt);

    float mapW = PlayScene::MapWidth  * PlayScene::BlockSize;
    float mapH = PlayScene::MapHeight * PlayScene::BlockSize;
    float halfW = GetBitmapWidth() / 2.0f;
    float halfH = GetBitmapHeight() / 2.0f;
    Position.x = std::clamp(Position.x, halfW, mapW - halfW + 8);
    Position.y = std::clamp(Position.y, halfH, mapH - halfH - 8);
}

void Player::UpdateMovement(float dt)
{
    Point v(0,0);
    if (keyW) v.y -= 1;
    if (keyS) v.y += 1;
    if (keyA){ v.x -= 1; faceRight=false; }
    if (keyD){ v.x += 1; faceRight=true;  }

    movingFlag = (v.x||v.y);
    if (!movingFlag) return;

    float len = std::sqrt(v.x*v.x + v.y*v.y);
    Position.x += (v.x/len) * moveSpeed * dt;
    Position.y += (v.y/len) * moveSpeed * dt;
}

void Player::UpdateAnimation(float dt)
{
    if (!movingFlag) return;
    animTimer += dt;
    if (animTimer >= animInterval) {
        animTimer = 0;
        currentFrame = (currentFrame+1) % framesRight.size();
        currentBitmap = framesRight[currentFrame];
    }
}

void Player::Draw() const
{
    if (!currentBitmap) return;
    ALLEGRO_BITMAP* bmp = currentBitmap.get();
    float w = al_get_bitmap_width(bmp);
    float h = al_get_bitmap_height(bmp);
    float scaleX = faceRight ? 1.0f : -1.0f;
    float anchorX = faceRight ? Anchor.x : (1.0f - Anchor.x);

    al_draw_tinted_scaled_rotated_bitmap(
        bmp, Tint,
        anchorX * w, Anchor.y * h,
        Position.x, Position.y,
        scaleX * (Size.x / w),
        Size.y  / h,
        Rotation, 0);
}

void Player::OnKeyDown(int k){ if(k==ALLEGRO_KEY_W)keyW=true; if(k==ALLEGRO_KEY_A)keyA=true;
                               if(k==ALLEGRO_KEY_S)keyS=true; if(k==ALLEGRO_KEY_D)keyD=true; }
void Player::OnKeyUp  (int k){ if(k==ALLEGRO_KEY_W)keyW=false;if(k==ALLEGRO_KEY_A)keyA=false;
                               if(k==ALLEGRO_KEY_S)keyS=false;if(k==ALLEGRO_KEY_D)keyD=false; }

void Player::TakeDamage(float d){ hp = std::max(0.0f, hp - d); }

void Player::OnMouseDown(int button, int mx, int my) {
}