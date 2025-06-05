//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.hpp"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "player_data.h"

void StartScene::Initialize()
{
    std::cout << nameInput << " " << player_uid << std::endl;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    std::string playerInfo = std::string("Player: ") + nameInput;
    AddNewObject(new Engine::Label(playerInfo, "pirulen.ttf", 24, w - 20, 20, 255, 255, 255, 255, 1.0, 0.0));

    std::string uidInfo = std::string("UID: ") + player_uid;
    AddNewObject(new Engine::Label(uidInfo, "pirulen.ttf", 20, w - 20, 50, 200, 200, 200, 255, 1.0, 0.0));

    AddNewObject(new Engine::Label("Tower Defense", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 200, halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 200, halfH / 2 + 250, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::LeaderBoardOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("LeaderBoard", "pirulen.ttf", 35, halfW, halfH / 2 + 300, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 200, halfH / 2 + 400, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));
}
void StartScene::Terminate()
{
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}
void StartScene::LeaderBoardOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("leaderboard-scene");
}
void StartScene::SettingsOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("setting-scene");
}