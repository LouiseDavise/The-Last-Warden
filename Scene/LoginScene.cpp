#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <sstream>
#include "LoginScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "player_data.h"

void LoginScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    uidInput = "";
    isInvalidUID = false;
    switch_key = false;

    AddNewObject(new Engine::Label("ENTER YOUR UID", "RealwoodRegular.otf", 70, halfW, 280, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Load your saved progress with your unique ID.", "RealwoodRegular.otf", 30, halfW, h / 6 + 150, 200, 200, 200, 255, 0.5, 0.5));

    auto *submit = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", (w - 480) / 2, 630, 480, 115);
    submit->SetOnClickCallback(std::bind(&LoginScene::OnSubmitClick, this, 0));
    AddNewControlObject(submit);
    AddNewObject(new Engine::Label("NEXT", "RealwoodRegular.otf", 56, halfW, 640 + 115 / 2, 255, 255, 255, 255, 0.5, 0.5));

    auto *back = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", (w - 480) / 2, 770, 480, 115);
    back->SetOnClickCallback(std::bind(&LoginScene::OnBackClick, this, 0));
    AddNewControlObject(back);
    AddNewObject(new Engine::Label("BACK", "RealwoodRegular.otf", 56, halfW, 780 + 115 / 2, 255, 255, 255, 255, 0.5, 0.5));
}

void LoginScene::Terminate()
{
    IScene::Terminate();
}

void LoginScene::Update(float)
{
}

void LoginScene::Draw() const
{
    IScene::Draw();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    float input_w = 600;
    float input_h = 90;
    float input_x = halfW - input_w / 2;
    float input_y = 450;

    al_draw_filled_rounded_rectangle(input_x, input_y, input_x + input_w, input_y + input_h, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(input_x, input_y, input_x + input_w, input_y + input_h, 10, 10, al_map_rgb(50, 50, 50), 3);

    // UID text
    auto font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 28).get();
    if (font)
        al_draw_text(font, al_map_rgb(30, 30, 30), halfW, input_y + input_h / 2 - 14, ALLEGRO_ALIGN_CENTER, uidInput.c_str());

    // Error message
    if (isInvalidUID)
    {
        al_draw_text(font, al_map_rgb(255, 0, 0), halfW, input_y + input_h + 30, ALLEGRO_ALIGN_CENTER, "Invalid UID. Please try again.");
    }
}

void LoginScene::OnKeyDown(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_BACKSPACE && !uidInput.empty() && !switch_key)
    {
        uidInput.pop_back();
        switch_key = true;
    }
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9 && uidInput.size() < 20 && !switch_key)
    {
        uidInput += '0' + (keyCode - ALLEGRO_KEY_0);
        switch_key = true;
    }
    else if (keyCode == ALLEGRO_KEY_ENTER)
    {
        OnSubmitClick(0);
    }
}

void LoginScene::OnKeyUp(int keyCode)
{
    if ((keyCode == ALLEGRO_KEY_BACKSPACE || (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)))
    {
        switch_key = false;
    }
}

bool LoginScene::ValidateUID(const std::string &uid, std::string &outName, std::string &outHero)
{
    std::ifstream file("Data/player_uid.txt");
    if (!file)
        return false;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string file_uid, file_name, file_hero;
        if (std::getline(ss, file_uid, ',') && std::getline(ss, file_name, ',') && std::getline(ss, file_hero))
        {
            if (file_uid == uid)
            {
                outName = file_name;
                outHero = file_hero;
                return true;
            }
        }
    }
    return false;
}

void LoginScene::OnSubmitClick(int)
{
    if (uidInput.empty())
        return;

    std::string loadedName;
    std::string loadedHero;
    if (ValidateUID(uidInput, loadedName, loadedHero))
    {
        strcpy_s(player_uid, sizeof(player_uid), uidInput.c_str());
        strcpy_s(nameInput, sizeof(nameInput), loadedName.c_str());
        strcpy_s(heroType, sizeof(heroType), loadedHero.c_str());
        Engine::GameEngine::GetInstance().ChangeScene("mode-select");
    }
    else
    {
        isInvalidUID = true;
    }
}

void LoginScene::OnBackClick(int)
{
    Engine::GameEngine::GetInstance().ChangeScene("auth-scene");
}
