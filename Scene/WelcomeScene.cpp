#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "WelcomeScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "player_data.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "StartScene.h"

void WelcomeScene::LoadPlayerHistory(const std::string &path, const std::string &uid)
{
    filteredEntries.clear();
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "Leaderboard file not found.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string file_uid, file_name, score_str, time, date;
        if (std::getline(ss, file_uid, ',') &&
            std::getline(ss, file_name, ',') &&
            std::getline(ss, score_str, ',') &&
            std::getline(ss, time, ',') &&
            std::getline(ss, date))
        {
            if (file_uid == uid)
            {
                LeaderboardEntry entry;
                entry.uid = file_uid;
                entry.name = file_name;
                entry.score = std::stoi(score_str);
                entry.time = time;
                entry.date = date;
                filteredEntries.push_back(entry);
            }
        }
    }
}

void WelcomeScene::OnContinueClicked()
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void WelcomeScene::Initialize()
{
    Engine::Point screen = Engine::GameEngine::GetInstance().GetScreenSize();
    int cx = screen.x / 2;

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    LoadPlayerHistory("Data/scoreboard.txt", player_uid);

    AddNewObject(new Engine::Label(std::string("Welcome back ") + nameInput + "!", "pirulen.ttf", 48, cx, 50, 146, 161, 185, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Take a look at your previous records", "pirulen.ttf", 24, cx, 120, 180, 180, 180, 255, 0.5, 0.5));
    float table_left = 300;
    float table_width = screen.x - 2 * table_left;
    float col_gap = table_width / 3;

    float col_name = table_left + col_gap * 0;
    float col_score = table_left + col_gap * 1;
    float col_time = table_left + col_gap * 2;
    float col_date = table_left + col_gap * 3;

    int header_y = 200;
    AddNewObject(new Engine::Label("Name", "pirulen.ttf", 24, col_name, header_y, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 24, col_score, header_y, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Time", "pirulen.ttf", 24, col_time, header_y, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Date", "pirulen.ttf", 24, col_date, header_y, 255, 255, 255, 255, 0.5, 0.5));

    int y = header_y + 40;
    if (filteredEntries.empty())
    {
        AddNewObject(new Engine::Label("No Record Found", "pirulen.ttf", 24, cx, screen.y / 2, 255, 0, 0, 255, 0.5, 0.5));
    }
    else
    {
        RenderPage();
    }

    auto *btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", cx - 200, 650, 400, 100);
    btn->SetOnClickCallback(std::bind(&WelcomeScene::OnContinueClicked, this));
    AddNewControlObject(btn);
    AddNewObject(btn);
    AddNewObject(new Engine::Label("CONTINUE", "pirulen.ttf", 36, cx, 700, 225, 225, 225, 255, 0.5, 0.5));
    auto *nextBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", cx + 150, 570, 100, 40);
    nextBtn->SetOnClickCallback(std::bind(&WelcomeScene::OnNextClick, this));
    AddNewControlObject(nextBtn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 20, cx + 200, 590, 0, 0, 0, 255, 0.5, 0.5));

    auto *prevBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", cx - 250, 570, 100, 40);
    prevBtn->SetOnClickCallback(std::bind(&WelcomeScene::OnBackClick, this));
    AddNewControlObject(prevBtn);
    AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 20, cx - 200, 590, 0, 0, 0, 255, 0.5, 0.5));
}

void WelcomeScene::Draw() const
{
    IScene::Draw();
}

void WelcomeScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = nullptr;
}

void WelcomeScene::RenderPage()
{
    Engine::Point screen = Engine::GameEngine::GetInstance().GetScreenSize();
    float table_left = 300;
    float table_width = screen.x - 2 * table_left;
    float col_gap = table_width / 3;

    float col_name = table_left + col_gap * 0;
    float col_score = table_left + col_gap * 1;
    float col_time = table_left + col_gap * 2;
    float col_date = table_left + col_gap * 3;

    int start = currentPage * entriesPerPage;
    int end = std::min(start + entriesPerPage, (int)filteredEntries.size());
    int y = 240;

    for (int i = start; i < end; i++)
    {
        const auto &entry = filteredEntries[i];
        auto *nameLabel = new Engine::Label(entry.name, "pirulen.ttf", 20, col_name, y, 200, 200, 200, 255, 0.5, 0.5);
        auto *scoreLabel = new Engine::Label(std::to_string(entry.score), "pirulen.ttf", 20, col_score, y, 200, 200, 200, 255, 0.5, 0.5);
        auto *timeLabel = new Engine::Label(entry.time, "pirulen.ttf", 20, col_time, y, 200, 200, 200, 255, 0.5, 0.5);
        auto *dateLabel = new Engine::Label(entry.date, "pirulen.ttf", 20, col_date, y, 200, 200, 200, 255, 0.5, 0.5);

        AddNewObject(nameLabel);
        AddNewObject(scoreLabel);
        AddNewObject(timeLabel);
        AddNewObject(dateLabel);

        pageLabels.push_back(nameLabel);
        pageLabels.push_back(scoreLabel);
        pageLabels.push_back(timeLabel);
        pageLabels.push_back(dateLabel);

        y += 40;
    }
}

void WelcomeScene::ClearPageLabels()
{
    for (auto *label : pageLabels)
    {
        RemoveObject(label->GetObjectIterator());
    }
    pageLabels.clear();
}

void WelcomeScene::OnNextClick()
{
    int maxPage = (filteredEntries.size() + entriesPerPage - 1) / entriesPerPage;
    if (currentPage < maxPage - 1)
    {
        currentPage++;
        ClearPageLabels();
        RenderPage();
    }
}

void WelcomeScene::OnBackClick()
{
    if (currentPage > 0)
    {
        currentPage--;
        ClearPageLabels();
        RenderPage();
    }
}
