#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <ctime>
#include <functional>
#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
#include "NewPlayerScene.hpp"
#include "Engine/Resources.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "StartScene.h"
#include <filesystem>
#include "player_data.h"

bool onEnterNameScene;
char nameInput[17] = "PLAYER";
int nameInputLen = 6;
char player_uid[20];

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

void save_player_uid_and_name(const std::string &uid, const std::string &player_name)
{
    std::filesystem::create_directories("Data"); 

    std::ofstream file("Data/player_uid.txt", std::ios::app); 
    std::cout << "Writing to: " << std::filesystem::absolute("Data/player_uid.txt") << std::endl;

    if (file.is_open())
    {
        file << uid << "," << player_name << "\n";
        file.close();
        std::cout << "Player UID and name appended to player_uid.txt\n";
    }
    else
    {
        std::cout << "Failed to append Player UID and name to file.\n";
    }
}

void NewPlayerScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    onEnterNameScene = true;

    // Title
    AddNewObject(new Engine::Label("Enter Your Name", "pirulen.ttf", 48, halfW, h / 6, 240, 240, 240, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("This will be displayed in the game.", "pirulen.ttf", 24, halfW, h / 6 + 60, 180, 180, 180, 255, 0.5, 0.5));

    // NEXT Button
    auto *btnNext = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, 520, 400, 100);
    btnNext->SetOnClickCallback(std::bind(&NewPlayerScene::OnNextClick, this, 0));
    AddNewControlObject(btnNext);
    AddNewObject(new Engine::Label("NEXT", "pirulen.ttf", 36, halfW, 570, 66, 76, 110, 255, 0.5, 0.5));

    // BACK Button
    auto *btnBack = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, 650, 400, 100);
    btnBack->SetOnClickCallback(std::bind(&NewPlayerScene::OnBackClick, this, 0));
    AddNewControlObject(btnBack);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 36, halfW, 700, 66, 76, 110, 255, 0.5, 0.5));
}

void NewPlayerScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void NewPlayerScene::OnNextClick(int)
{
    if (nameInputLen >= 3)
    {
        generate_player_uid(nameInput, player_uid);
        save_player_uid_and_name(player_uid, nameInput);
        onEnterNameScene = false;
        Engine::GameEngine::GetInstance().ChangeScene("start");
    }
}

void NewPlayerScene::OnBackClick(int)
{
    Engine::GameEngine::GetInstance().ChangeScene("auth-scene");
}

// Handles key input during name entry
void NewPlayerScene::OnKeyDown(int keyCode)
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
}

void NewPlayerScene::Draw() const
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
    {
        al_draw_text(font, al_map_rgb(30, 30, 30), halfW, input_y + 25, ALLEGRO_ALIGN_CENTER, nameInput);
    }
}
