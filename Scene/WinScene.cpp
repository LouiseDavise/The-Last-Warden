#include <functional>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <ctime>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "player_data.h"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

void WinScene::Initialize()
{
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));

    PlayScene *play = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    // Score Calculation
    int score = play->GetLives() * 100 + play->GetMoney();

    std::string summary = "Score: " + std::to_string(score) + " | Time: " + std::to_string(static_cast<int>(play->matchTime)) + "s";
    AddNewObject(new Engine::Label(summary, "pirulen.ttf", 36, halfW, halfH / 2 - 20, 255, 255, 255, 255, 0.5, 0.5));

    std::filesystem::create_directories("Data");
    std::ofstream file("Data/scoreboard.txt", std::ios::app);
    std::cout << "Writing to: " << std::filesystem::absolute("Data/scoreboard.txt") << std::endl;

    if (file.is_open())
    {
        std::time_t now = std::time(nullptr);
        char dateStr[11];
        std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", std::localtime(&now));

        file << player_uid << "," << nameInput << "," << score << ","
             << static_cast<int>(play->matchTime) << "s" << "," << dateStr << "\n";
        file.close();
        std::cout << "Score saved to scoreboard.txt\n";
    }
    else
    {
        std::cout << "Failed to write score to file.\n";
    }

    if (play && play->MapId < 2)
    {
        Engine::ImageButton *backBtn;
        backBtn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 50, halfH * 7 / 4 - 50, 400, 100);
        backBtn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
        AddNewControlObject(backBtn);
        AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW + 250, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
        Engine::ImageButton *nextBtn;
        nextBtn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 440, halfH * 7 / 4 - 50, 400, 100);
        nextBtn->SetOnClickCallback([this]()
                                    {
        PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
        if (scene && scene->MapId < 3) {
            scene->MapId++;
            Engine::GameEngine::GetInstance().ChangeScene("play");
        } else {
            Engine::GameEngine::GetInstance().ChangeScene("stage-select");
        } });
        AddNewControlObject(nextBtn);
        AddNewObject(new Engine::Label("Next Stage", "pirulen.ttf", 36, halfW - 240, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    }
    else
    {
        Engine::ImageButton *backBtn;
        backBtn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
        backBtn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
        AddNewControlObject(backBtn);
        AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    }
    bgmId = AudioHelper::PlayAudio("win.wav");
}

void WinScene::Terminate()
{
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}

void WinScene::Update(float deltaTime)
{
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2)
    {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}

void WinScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
