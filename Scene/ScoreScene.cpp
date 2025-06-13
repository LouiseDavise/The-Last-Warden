#include <string>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <cstdio>

#include "ScoreScene.hpp"
#include "PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Engine/AudioHelper.hpp"
#include "player_data.h"

void ScoreScene::Initialize()
{
    int screenW = al_get_display_width(al_get_current_display());
    int screenH = al_get_display_height(al_get_current_display());
    float centerX = screenW / 2;
    float startY = screenH / 2 - 180;
    float spacing = 45;

    displayScore = 0;
    scoreTimer = 0;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, screenW, screenH));

    PlayScene *play = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    int time = play ? static_cast<int>(play->matchTime) : 0;
    int killCount = play ? play->GetKillCount() : 0;
    int waves = play ? play->GetWaveCount() : 0;
    int goldLeft = play ? play->GetMoney() : 0;
    int goldSpent = play ? play->GetTotalMoneySpent() : 0;
    std::string playerName = nameInput;

    // ✅ Score calculation
    score =
        goldSpent * 2 +
        killCount * 10 +
        waves * 150 +
        time * 1 +
        goldLeft / 10;

    // Format time to HH:MM:SS
    int hours = time / 3600;
    int minutes = (time % 3600) / 60;
    int seconds = time % 60;
    char timeStr[16];
    std::snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hours, minutes, seconds);

    // Save to file
    std::filesystem::create_directories("Data");
    std::ofstream file("Data/scoreboard.txt", std::ios::app);
    if (file.is_open())
    {
        std::time_t now = std::time(nullptr);
        char dateStr[11];
        std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", std::localtime(&now));
        file << player_uid << "," << playerName << "," << score << "," << timeStr << ","
             << killCount << "," << waves << "," << goldLeft << "," << dateStr << "\n";
        file.close();
    }

    // UI Elements
    AddNewObject(new Engine::Label("GAME SUMMARY", "RealwoodRegular.otf", 84, centerX, startY - 150, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "RealwoodRegular.otf", 38, centerX, startY - 100, 200, 200, 255, 255, 0.5, 0.5));

    scoreLabel = new Engine::Label(std::to_string(static_cast<int>(0)), "RealwoodRegular.otf", 238, centerX, startY + 50, 255, 215, 0, 255, 0.5, 0.5);
    AddNewObject(scoreLabel);

    // Info display
    AddNewObject(new Engine::Label("Time: " + std::string(timeStr), "RealwoodRegular.otf", 38, centerX, startY - 20 + spacing * 4.0, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Enemies Defeated: " + std::to_string(killCount), "RealwoodRegular.otf", 38, centerX, startY - 20 + spacing * 5.0, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Waves Survived: " + std::to_string(waves), "RealwoodRegular.otf", 38, centerX, startY - 20 + spacing * 6.0, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Gold: " + std::to_string(goldLeft), "RealwoodRegular.otf", 38, centerX, startY - 20 + spacing * 7.0, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Player: " + playerName, "RealwoodRegular.otf", 34, centerX, startY - 20 + spacing * 8.0, 200, 255, 200, 255, 0.5, 0.5));

    // Back Button
    backButton = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", centerX - 240, 770, 480, 115);
    backButton->SetOnClickCallback([]()
                                   { Engine::GameEngine::GetInstance().ChangeScene("mode-select"); });
    AddNewControlObject(backButton);
    AddNewObject(new Engine::Label("BACK", "RealwoodRegular.otf", 56, centerX, 780 + 115 / 2, 255, 255, 255, 255, 0.5, 0.5));
}

void ScoreScene::Terminate()
{
    IScene::Terminate();
}

void ScoreScene::Update(float deltaTime)
{
    scoreTimer += deltaTime;
    if (scoreTimer < scoreDuration)
    {
        float t = scoreTimer / scoreDuration;
        displayScore = t * score;
    }
    else
    {
        displayScore = score;
    }
    scoreLabel->Text = std::to_string(static_cast<int>(displayScore));
}
