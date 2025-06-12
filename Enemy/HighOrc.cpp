#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "HighOrc.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

HighOrc::HighOrc(float x, float y)
    : Enemy("Enemies/HighOrc/Run/image1x1.png", x, y, 32, 85, 250, 250, 20, 0.75f, 50)
{
    Size.x = 128;
    Size.y = 128;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/HighOrc/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/HighOrc/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 3; i <= 8; ++i)
    {
        std::string path = "Enemies/HighOrc/Attack/image" + std::to_string(i) + "x1.png";
        attackFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 6; ++i)
    {
        std::string path = "Enemies/HighOrc/Hurt/image" + std::to_string(i) + "x1.png";
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