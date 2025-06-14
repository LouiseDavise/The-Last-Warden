#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <ctime>
#include <functional>
#include <string>
#include <filesystem>
#include <cstdio>
#include <cstring>
#include <fstream>
#include "RegisterScene.hpp"
#include "Engine/Resources.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include <filesystem>
#include "player_data.h"

bool onEnterNameScene;

unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    hash += static_cast<unsigned long>(time(nullptr));
    return hash;
}

void generate_player_uid(const char *player_name, char *uid_buffer)
{
    unsigned long hashed_value = hash(player_name);
    snprintf(uid_buffer, 20, "%lu", hashed_value);
}


void RegisterScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    onEnterNameScene = true;

    // Title
    AddNewObject(new Engine::Label("ENTER YOUR NAME", "RealwoodRegular.otf", 80, halfW, 280, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("This will be displayed in the game.", "pirulen.ttf", 24, halfW, h / 6 + 150, 180, 180, 180, 255, 0.5, 0.5));

    // NEXT Button
    auto *btnNext = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", (w - 480) / 2, 630, 480, 115);
    btnNext->SetOnClickCallback(std::bind(&RegisterScene::OnNextClick, this, 0));
    AddNewControlObject(btnNext);
    AddNewObject(new Engine::Label("NEXT", "RealwoodRegular.otf", 56, halfW, 640 + 115 / 2, 255, 255, 255, 255, 0.5, 0.5));

    // BACK Button
    auto *btnBack = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", (w - 480) / 2, 770, 480, 115);
    btnBack->SetOnClickCallback(std::bind(&RegisterScene::OnBackClick, this, 0));
    AddNewControlObject(btnBack);
    AddNewObject(new Engine::Label("BACK", "RealwoodRegular.otf", 56, halfW, 780 + 115 / 2, 255, 255, 255, 255, 0.5, 0.5));
}

void RegisterScene::Terminate()
{
    IScene::Terminate();
}

void RegisterScene::OnNextClick(int)
{
    if (nameInputLen >= 3)
    {
        generate_player_uid(nameInput, player_uid);
        onEnterNameScene = false;
        Engine::GameEngine::GetInstance().ChangeScene("select-hero");
    }
}

void RegisterScene::OnBackClick(int)
{
    Engine::GameEngine::GetInstance().ChangeScene("auth-scene");
}

// Handles key input during name entry
void RegisterScene::OnKeyDown(int keyCode)
{
    if (!onEnterNameScene)
        return;

    if (keyCode == ALLEGRO_KEY_BACKSPACE && nameInputLen > 0)
    {
        nameInput[--nameInputLen] = '\0';
    }
    else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && nameInputLen < 16)
    {
        nameInput[nameInputLen++] = 'A' + (keyCode - ALLEGRO_KEY_A);
        nameInput[nameInputLen] = '\0';
    }
    else if (keyCode == ALLEGRO_KEY_ENTER)
    {
        OnNextClick(0);
    }
}

void RegisterScene::Draw() const
{
    IScene::Draw();

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    float input_w = 600;
    float input_h = 90;
    float input_x = halfW - input_w / 2;
    float input_y = 450;

    // Draw input box
    al_draw_filled_rounded_rectangle(input_x, input_y, input_x + input_w, input_y + input_h, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(input_x, input_y, input_x + input_w, input_y + input_h, 10, 10, al_map_rgb(50, 50, 50), 3);

    auto font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 28).get();
    if (font)
    {
        al_draw_text(font, al_map_rgb(30, 30, 30), halfW, input_y + input_h / 2 - 14, ALLEGRO_ALIGN_CENTER, nameInput);
    }
}
