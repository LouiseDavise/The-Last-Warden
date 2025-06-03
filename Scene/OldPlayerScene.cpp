#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <sstream>
#include "OldPlayerScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "player_data.h"

void OldPlayerScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    uidInput = "";
    isInvalidUID = false;
    switch_key = false;

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    AddNewObject(new Engine::Label("Enter Your UID", "pirulen.ttf", 48, halfW, 100, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Enter your UID to access your game data.", "pirulen.ttf", 24, halfW, 160, 180, 180, 180, 255, 0.5, 0.5));

    auto *submit = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, 520, 400, 100);
    submit->SetOnClickCallback(std::bind(&OldPlayerScene::OnSubmitClick, this, 0));
    AddNewControlObject(submit);
    AddNewObject(new Engine::Label("NEXT", "pirulen.ttf", 36, halfW, 570, 66, 76, 110, 255, 0.5, 0.5));

    auto *back = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, 650, 400, 100);
    back->SetOnClickCallback(std::bind(&OldPlayerScene::OnBackClick, this, 0));
    AddNewControlObject(back);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 36, halfW, 700, 66, 76, 110, 255, 0.5, 0.5));
}

void OldPlayerScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = nullptr;
    IScene::Terminate();
}

void OldPlayerScene::Update(float)
{
}

void OldPlayerScene::Draw() const
{
    IScene::Draw();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    float input_x = halfW - 300;
    float input_y = 360 - 50;
    float input_w = halfW - 190;
    float input_h = 90;

    al_draw_filled_rounded_rectangle(input_x, input_y, input_x + input_w, input_y + input_h, 10, 10, al_map_rgb(240, 240, 240));
    al_draw_rounded_rectangle(input_x, input_y, input_x + input_w, input_y + input_h, 10, 10, al_map_rgb(50, 50, 50), 3);

    auto font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 28).get();
    if (font)
        al_draw_text(font, al_map_rgb(30, 30, 30), halfW, input_y + 25, ALLEGRO_ALIGN_CENTER, uidInput.c_str());

    if (isInvalidUID)
    {
        al_draw_text(font, al_map_rgb(255, 0, 0), halfW, input_y + 130, ALLEGRO_ALIGN_CENTER, "Invalid UID. Please try again.");
    }
}

void OldPlayerScene::OnKeyDown(int keyCode)
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
}

void OldPlayerScene::OnKeyUp(int keyCode)
{
    if ((keyCode == ALLEGRO_KEY_BACKSPACE || (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)))
    {
        switch_key = false;
    }
}

bool OldPlayerScene::ValidateUID(const std::string &uid, std::string &outName)
{
    std::ifstream file("Data/player_uid.txt");
    if (!file)
        return false;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string file_uid, file_name;
        if (std::getline(ss, file_uid, ',') && std::getline(ss, file_name))
        {
            if (file_uid == uid)
            {
                outName = file_name;
                return true;
            }
        }
    }
    return false;
}

void OldPlayerScene::OnSubmitClick(int)
{
    if (uidInput.empty())
        return;

    std::string loadedName;
    if (ValidateUID(uidInput, loadedName))
    {
        strcpy_s(player_uid, sizeof(player_uid), uidInput.c_str());
        strcpy_s(nameInput, sizeof(nameInput), loadedName.c_str());
        Engine::GameEngine::GetInstance().ChangeScene("welcome-scene");
    }
    else
    {
        isInvalidUID = true;
    }
}

void OldPlayerScene::OnBackClick(int)
{
    Engine::GameEngine::GetInstance().ChangeScene("auth-scene");
}
