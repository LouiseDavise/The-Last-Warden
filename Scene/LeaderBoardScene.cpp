#include <allegro5/allegro_audio.h>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <algorithm>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "LeaderBoardScene.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "player_data.h"

void LeaderBoardScene::Initialize()
{
    currentPage = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    LoadLeaderboard();
    SortLeaderboard();

    AddNewObject(new Engine::Label("LEADERBOARD", "pirulen.ttf", 48, halfW, h / 8 + 20, 255, 255, 255, 255, 0.5, 0.5));

    Engine::ImageButton *nextBtn = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW + 250, halfH / 2 + 375, 100, 50);
    nextBtn->SetOnClickCallback(std::bind(&LeaderBoardScene::OnNextClick, this));
    AddNewControlObject(nextBtn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 24, halfW + 300, halfH / 2 + 400, 0, 0, 0, 255, 0.5, 0.5));

    Engine::ImageButton *btn = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 200, halfH / 2 + 350, 400, 100);
    btn->SetOnClickCallback(std::bind(&LeaderBoardScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 36, halfW, halfH / 2 + 400, 0, 0, 0, 255, 0.5, 0.5));

    Engine::ImageButton *prevBtn = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 350, halfH / 2 + 375, 100, 50);
    prevBtn->SetOnClickCallback(std::bind(&LeaderBoardScene::OnBackClick, this));
    AddNewControlObject(prevBtn);
    AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 24, halfW - 300, halfH / 2 + 400, 0, 0, 0, 255, 0.5, 0.5));

    RenderPage();
}

void LeaderBoardScene::Terminate()
{
    ClearPageLabels();
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void LeaderBoardScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void LeaderBoardScene::Update(float deltaTime) {}

void LeaderBoardScene::LoadLeaderboard()
{
    leaderboard.clear();
    std::ifstream file("Data/scoreboard.txt");
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string uid, name, scoreStr, time, date;
        std::getline(ss, uid, ',');
        std::getline(ss, name, ',');
        std::getline(ss, scoreStr, ',');
        std::getline(ss, time, ',');
        std::getline(ss, date, ',');

        LeaderboardEntry entry;
        entry.uid = uid;
        entry.name = name;
        entry.score = std::stoi(scoreStr);
        entry.time = time;
        entry.date = date;
        leaderboard.push_back(entry);
    }
    file.close();
}

void LeaderBoardScene::SortLeaderboard()
{
    std::sort(leaderboard.begin(), leaderboard.end(), [](const LeaderboardEntry &a, const LeaderboardEntry &b)
              { return a.score > b.score; });
}

void LeaderBoardScene::OnNextClick()
{
    int maxPage = (leaderboard.size() + entriesPerPage - 1) / entriesPerPage;
    if (currentPage < maxPage - 1)
    {
        currentPage++;
        ClearPageLabels();
        RenderPage();
    }
}

void LeaderBoardScene::OnBackClick()
{
    if (currentPage > 0)
    {
        currentPage--;
        ClearPageLabels();
        RenderPage();
    }
}

void LeaderBoardScene::RenderPage()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

    float box_x = 50;
    float box_y = h / 8 - 30;
    float box_width = w - 100;
    float col_rank = box_x + 60;
    float col_name = box_x + box_width * 0.22;
    float col_score = box_x + box_width * 0.48;
    float col_time = box_x + box_width * 0.68;
    float col_date = box_x + box_width * 0.85;

    int start = currentPage * entriesPerPage;
    int end = std::min(start + entriesPerPage, (int)leaderboard.size());

    for (int i = start; i < end; i++)
    {
        float row_y = box_y + 200 + (i - start) * 50;
        auto color = (leaderboard[i].uid == player_uid) ? std::make_tuple(255, 223, 0) : std::make_tuple(255, 255, 255);

        auto *rankLabel = new Engine::Label(std::to_string(i + 1), "pirulen.ttf", 24, col_rank, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(rankLabel);
        pageLabels.push_back(rankLabel);

        auto *nameLabel = new Engine::Label(leaderboard[i].name, "pirulen.ttf", 24, col_name, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(nameLabel);
        pageLabels.push_back(nameLabel);

        auto *scoreLabel = new Engine::Label(std::to_string(leaderboard[i].score), "pirulen.ttf", 24, col_score, row_y,
                                             std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(scoreLabel);
        pageLabels.push_back(scoreLabel);

        auto *timeLabel = new Engine::Label(leaderboard[i].time, "pirulen.ttf", 24, col_time, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(timeLabel);
        pageLabels.push_back(timeLabel);

        auto *dateLabel = new Engine::Label(leaderboard[i].date, "pirulen.ttf", 24, col_date, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(dateLabel);
        pageLabels.push_back(dateLabel);
    }
}

void LeaderBoardScene::ClearPageLabels()
{
    for (auto *label : pageLabels)
    {
        RemoveObject(label->GetObjectIterator());
    }
    pageLabels.clear();
}
