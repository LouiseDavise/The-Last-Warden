#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Orc.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

Orc::Orc(float x, float y)
    : Enemy("Enemies/Orc/Run/image1x1.png", x, y, 48, 120, 150, 150, 15, 0.6f, 35)
{
    Size.x = 192;
    Size.y = 192;
    idleMark = 6;
    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/Orc/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/Orc/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 10; ++i)
    {
        std::string path;
        if(i <= idleMark) path = "Enemies/Orc/Attack/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/Orc/Idle/image" + std::to_string(i-idleMark) + "x1.png";
        attackFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 6; ++i)
    {
        std::string path = "Enemies/Orc/Hurt/image" + std::to_string(i) + "x1.png";
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