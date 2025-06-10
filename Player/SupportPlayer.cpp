#include "SupportPlayer.hpp"
#include "Scene/PlayScene.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"

using Res = Engine::Resources;

SupportPlayer::SupportPlayer(float x, float y)
    : Player(x, y, 100.0f, 190.0f, "Characters/Support/image1x1.png", "image", 10, 10.0f)
{
    // Load idle frames (1-4)
    for (int i = 1; i <= 4; ++i)
        idleFrames.push_back(Res::GetInstance().GetBitmap("Characters/Support/image" + std::to_string(i) + "x1.png"));

    // Load walking frames (5-10)
    for (int i = 5; i <= 10; ++i)
        walkFrames.push_back(Res::GetInstance().GetBitmap("Characters/Support/image" + std::to_string(i) + "x1.png"));

    currentBitmap = idleFrames[0];
}

void SupportPlayer::Update(float dt)
{
    if (IsDead())
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
            if (scene->IsWalkable(gx, gy))
            {
                Position.x = nextX;
                Position.y = nextY;
            }
        }
    }

    // Animation update
    animTimer += dt;
    const auto &activeFrames = movingFlag ? walkFrames : idleFrames;
    if (animTimer >= animInterval && !activeFrames.empty())
    {
        animTimer = 0;
        currentFrame = (currentFrame + 1) % activeFrames.size();
        currentBitmap = activeFrames[currentFrame];
    }

    Sprite::Update(dt);
}

Engine::Point SupportPlayer::PositionWeapon() const
{
    float offX = FacingRight() ? 2 : -2;
    return Engine::Point{Position.x + offX, Position.y + 7};
}

void SupportPlayer::OnKeyDown(int k)
{
    if (k == ALLEGRO_KEY_UP)
        keyUp = true;
    if (k == ALLEGRO_KEY_DOWN)
        keyDown = true;
    if (k == ALLEGRO_KEY_LEFT)
        keyLeft = true;
    if (k == ALLEGRO_KEY_RIGHT)
        keyRight = true;
}

void SupportPlayer::OnKeyUp(int k)
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
