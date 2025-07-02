#include "LogoScene.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

void LogoScene::Initialize()
{
    timer = 0.0f;
    logoBitmap = al_load_bitmap("Resource/images/Logo/logo.png");
    if (!logoBitmap)
    {
        throw std::runtime_error("Failed to load logo bitmap.");
    }
}

void LogoScene::Terminate()
{
    if (logoBitmap)
    {
        al_destroy_bitmap(logoBitmap);
        logoBitmap = nullptr;
    }
}

void LogoScene::Update(float deltaTime)
{
    timer += deltaTime;

    if (timer >= 3.5f)
    {
        Engine::GameEngine::GetInstance().ChangeScene("auth-scene");
        return;
    }

    if (timer < 1.0f)
    {
        float t = timer / 1.0f;
        logoScale = 0.5f + t * (1.7f - 0.5f);
        logoAlpha = 1.0f;
    }
    else if (timer < 2.5f)
    {
        logoScale = 1.7f;
        logoAlpha = 1.0f;
    }
    else
    {
        float t = (timer - 2.5f) / 1.0f;
        logoScale = 1.7f;
        logoAlpha = 1.0f - t;
    }
}

void LogoScene::Draw() const
{
    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (logoBitmap)
    {
        int bmpW = al_get_bitmap_width(logoBitmap);
        int bmpH = al_get_bitmap_height(logoBitmap);
        int screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;

        float drawW = bmpW * logoScale;
        float drawH = bmpH * logoScale;
        float drawX = (screenW - drawW) / 2;
        float drawY = (screenH - drawH) / 2;

        al_draw_tinted_scaled_bitmap(
            logoBitmap,
            al_map_rgba_f(1, 1, 1, logoAlpha),
            0, 0, bmpW, bmpH,
            drawX, drawY - 140, drawW, drawH,
            0);
    }
}
