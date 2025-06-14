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

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    LoadLeaderboard();
    SortLeaderboard();

    AddNewObject(new Engine::Label("LEADERBOARD", "RealwoodRegular.otf", 78, halfW, h / 8 + 20, 255, 255, 255, 255, 0.5, 0.5));

    Engine::ImageButton *nextBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW + 250, halfH / 2 + 475, 100, 50);
    nextBtn->SetOnClickCallback(std::bind(&LeaderBoardScene::OnNextClick, this));
    AddNewControlObject(nextBtn);
    AddNewObject(new Engine::Label("Next", "RealwoodRegular.otf", 24, halfW + 300, halfH / 2 + 500, 255, 255, 255, 255, 0.5, 0.5));

    Engine::ImageButton *btn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW - 200, halfH / 2 + 450, 400, 100);
    btn->SetOnClickCallback(std::bind(&LeaderBoardScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "RealwoodRegular.otf", 56, halfW, halfH / 2 + 505, 255, 255, 255, 255, 0.5, 0.5));

    Engine::ImageButton *prevBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW - 350, halfH / 2 + 475, 100, 50);
    prevBtn->SetOnClickCallback(std::bind(&LeaderBoardScene::OnBackClick, this));
    AddNewControlObject(prevBtn);
    AddNewObject(new Engine::Label("Prev", "RealwoodRegular.otf", 24, halfW - 300, halfH / 2 + 500, 255, 255, 255, 255, 0.5, 0.5));

    RenderPage();
}

void LeaderBoardScene::Terminate()
{
    ClearPageLabels();
    IScene::Terminate();
}

void LeaderBoardScene::BackOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
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

    float box_width = w * 0.9;
    float box_x = (w - box_width) / 2 + 100;
    float box_y = h / 8 + 50;
    float entryHeight = 65;
    float headerY = box_y + 110;
    float startY = headerY + 75;

    // Column positions
    float col_rank = box_x + 60;
    float col_name = box_x + box_width * 0.2;
    float col_score = box_x + box_width * 0.45;
    float col_time = box_x + box_width * 0.65;
    float col_date = box_x + box_width * 0.82;

    // Add table headers
    std::vector<std::tuple<std::string, float>> headers = {
        {"RANK", col_rank},
        {"NAME", col_name},
        {"SCORE", col_score},
        {"DURATION", col_time},
        {"DATE", col_date}};

    for (auto &header : headers)
    {
        auto *label = new Engine::Label(std::get<0>(header), "RealwoodRegular.otf", 46, std::get<1>(header), headerY,
                                        255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(label);
        pageLabels.push_back(label);
    }

    int start = currentPage * entriesPerPage;
    int end = std::min(start + entriesPerPage, static_cast<int>(leaderboard.size()));

    for (int i = start; i < end; i++)
    {
        float row_y = startY + (i - start) * entryHeight;
        auto color = (leaderboard[i].uid == player_uid) ? std::make_tuple(255, 223, 0) : std::make_tuple(232, 232, 232);

        auto *rankLabel = new Engine::Label(std::to_string(i + 1), "RealwoodRegular.otf", 34, col_rank, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(rankLabel);
        pageLabels.push_back(rankLabel);

        auto *nameLabel = new Engine::Label(leaderboard[i].name, "RealwoodRegular.otf", 34, col_name, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(nameLabel);
        pageLabels.push_back(nameLabel);

        auto *scoreLabel = new Engine::Label(std::to_string(leaderboard[i].score), "RealwoodRegular.otf", 34, col_score, row_y,
                                             std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(scoreLabel);
        pageLabels.push_back(scoreLabel);

        auto *timeLabel = new Engine::Label(leaderboard[i].time, "RealwoodRegular.otf", 34, col_time, row_y,
                                            std::get<0>(color), std::get<1>(color), std::get<2>(color), 255, 0.5, 0.5);
        AddNewObject(timeLabel);
        pageLabels.push_back(timeLabel);

        auto *dateLabel = new Engine::Label(leaderboard[i].date, "RealwoodRegular.otf", 34, col_date, row_y,
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
