#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "HighOrc.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

HighOrc::HighOrc(float x, float y)
    : Enemy("Enemies/HighOrc/Run/image1x1.png", x, y, 48, 85, 500, 500, 30, 0.75f, 50)
{
    Size.x = 192;
    Size.y = 192;
    idleMark = 6;
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

    for (int i = 1; i <= 10; ++i)
    {
        std::string path;
        if(i <= idleMark) path = "Enemies/HighOrc/Attack/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/HighOrc/Idle/image" + std::to_string(i-idleMark) + "x1.png";
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
    attackInterval = 0.055f;
    hurtTimer = 0;
    hurtInterval = 0.05f;
    currentFrame = 0;
}