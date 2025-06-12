#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "ToxicSlime.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

ToxicSlime::ToxicSlime(float x, float y)
    : Enemy("Enemies/ToxicSlime/Run/image1x1.png", x, y, 32, 32, 60, 60, 5, 1.0f, 10)
{
    Size.x = 128;
    Size.y = 72;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/ToxicSlime/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 10; ++i)
    {
        std::string path = "Enemies/ToxicSlime/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 4; i <= 11; ++i)
    {
        std::string path = "Enemies/ToxicSlime/Attack/image" + std::to_string(i) + "x1.png";
        attackFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 5; ++i)
    {
        std::string path = "Enemies/ToxicSlime/Hurt/image" + std::to_string(i) + "x1.png";
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