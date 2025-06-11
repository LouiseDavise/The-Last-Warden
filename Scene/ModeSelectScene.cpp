#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "ModeSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "player_data.h"

void ModeSelectScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    // Layout parameters
    const int buttonWidth = 480;
    const int buttonHeight = 115;
    const int spacing = 35;
    const int titleHeight = 100;

    const int totalHeight =
        titleHeight + spacing +
        buttonHeight + spacing +
        buttonHeight;

    int startY = (h - totalHeight - 200) / 2;

    // User Info
    std::string profileImagePath;
    if (std::string(heroType) == "MAGE")
        profileImagePath = "Characters/mage-profile.png";
    else if (std::string(heroType) == "ARCHER")
        profileImagePath = "Characters/archer-profile.png";

    AddNewObject(new Engine::Image("UI/avatar.png", 0, 0, 120, 120));

    Engine::Image *profileImage = new Engine::Image(profileImagePath, 35, 30, 55, 55);
    AddNewObject(profileImage);

    // std::string playerInfo = std::string("Player: ") + nameInput;
    AddNewObject(new Engine::Label(nameInput, "pirulen.ttf", 27, 115, 30, 255, 255, 255, 255, 0.0, 0.0));

    std::string uidInfo = std::string("UID: ") + player_uid;
    AddNewObject(new Engine::Label(uidInfo, "pirulen.ttf", 16, 115, 60, 200, 200, 200, 255, 0.0, 0.0));

    // Title
    AddNewObject(new Engine::Label("SELECT MODE", "RealwoodRegular.otf", 72, halfW, startY + titleHeight / 2, 255, 255, 255, 255, 0.5, 0.5));
    startY += titleHeight + spacing;

    // 1-PLAYER Button
    auto *btn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", (w - buttonWidth) / 2, startY, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("1-PLAYER", "RealwoodRegular.otf", 56, halfW, startY + buttonHeight / 2, 255, 255, 255, 255, 0.5, 0.5));
    startY += buttonHeight + spacing;

    // 2-PLAYER Button
    btn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", (w - buttonWidth) / 2, startY, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("2-PLAYER", "RealwoodRegular.otf", 56, halfW, startY + buttonHeight / 2, 255, 255, 255, 255, 0.5, 0.5));

    // BGM
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ModeSelectScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void ModeSelectScene::PlayOnClick(int stage)
{
    auto &engine = Engine::GameEngine::GetInstance();
    PlayScene *scene = dynamic_cast<PlayScene *>(engine.GetScene("play"));
    if (scene)
    {
        if (stage == 1)
        {
            scene->SetMapFile("Resource/map1.txt");
            scene->SetWaveFile("Resource/single_wave.txt");
            scene->SetTwoPlayerMode(false);
        }
        else if (stage == 2)
        {
            scene->SetMapFile("Resource/map2.txt");
            scene->SetWaveFile("Resource/duo_wave.txt");
            scene->SetTwoPlayerMode(true);
        }
    }
    engine.ChangeScene("play");
}

void ModeSelectScene::ScoreboardOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}