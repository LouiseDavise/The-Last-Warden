#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
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
    fadingToPlay = false;
    stopBGMOnTerminate = false;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));
    if (cameFromScoreScene)
    {
        Engine::GameEngine::GetInstance().GlobalBGMInstance =
            AudioHelper::PlaySample(current_bgm, true, AudioHelper::BGMVolume);
    }

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

    int buttonSize = 64;
    int padding = 16;

    // Leaderboard button
    auto *leaderboardBtn = new Engine::ImageButton(
        "UI/leaderboard-button.png", "UI/leaderboard-button.png",
        w - 2 * (buttonSize + padding), padding,
        buttonSize, buttonSize);
    leaderboardBtn->SetOnClickCallback(std::bind(&ModeSelectScene::ScoreboardOnClick, this));
    AddNewControlObject(leaderboardBtn);

    // Settings button
    auto *settingsBtn = new Engine::ImageButton(
        "UI/setting-button.png", "UI/setting-button.png",
        w - (buttonSize + padding), padding,
        buttonSize, buttonSize);
    settingsBtn->SetOnClickCallback(std::bind(&ModeSelectScene::SettingsOnClick, this));

    AddNewControlObject(settingsBtn);

    // User Info
    std::string profileImagePath;
    if (std::string(heroType) == "MAGE")
        profileImagePath = "Characters/Mage/mage-profile.png";
    else if (std::string(heroType) == "ARCHER")
        profileImagePath = "Characters/Archer/archer-profile.png";
    else if (std::string(heroType) == "SPEARMAN")
        profileImagePath = "Characters/Spearman/spearman-profile.png";

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
    auto *btn = new Engine::ImageButton("UI/button.png", "UI/button.png", (w - buttonWidth) / 2, startY, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("1-PLAYER", "RealwoodRegular.otf", 56, halfW, 10 + startY + buttonHeight / 2, 255, 255, 255, 255, 0.5, 0.5));
    startY += buttonHeight + spacing;

    // 2-PLAYER Button
    btn = new Engine::ImageButton("UI/button.png", "UI/button.png", (w - buttonWidth) / 2, startY, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("2-PLAYER", "RealwoodRegular.otf", 56, halfW, 10 + startY + buttonHeight / 2, 255, 255, 255, 255, 0.5, 0.5));
}

void ModeSelectScene::Terminate()
{
    IScene::Terminate();
}

void ModeSelectScene::PlayOnClick(int mode)
{
    auto &engine = Engine::GameEngine::GetInstance();
    PlayScene *scene = dynamic_cast<PlayScene *>(engine.GetScene("play"));
    if (scene)
    {
        if (mode == 1)
        {
            auto &engine = Engine::GameEngine::GetInstance();

            if (engine.GlobalBGMInstance)
            {
                AudioHelper::StopSample(engine.GlobalBGMInstance);
                engine.GlobalBGMInstance = nullptr;
            }
            scene->SetMapFile("Resource/forest_map.txt");
            scene->SetWaveFile("Resource/single_wave.txt");
            scene->SetTwoPlayerMode(false);
            if (!fadingToPlay)
            {
                fadingToPlay = true;
                fadeTimer = 0;
            }
        }
        else if (mode == 2)
        {
            scene->SetMapFile("Resource/forest_map.txt");
            scene->SetWaveFile("Resource/duo_wave.txt");
            scene->SetTwoPlayerMode(true);
            engine.ChangeScene("select-companion");
        }
    }
}

void ModeSelectScene::ScoreboardOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("leaderboard-scene");
}

void ModeSelectScene::SettingsOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("setting-scene");
}

void ModeSelectScene::Update(float deltaTime)
{
    if (fadingToPlay)
    {
        fadeTimer += deltaTime;
        if (fadeTimer >= fadeDuration)
        {
            fadeTimer = fadeDuration;
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }
    }
    IScene::Update(deltaTime);
}

void ModeSelectScene::Draw() const
{
    IScene::Draw();
    if (fadingToPlay)
    {
        float alpha = std::min(fadeTimer / fadeDuration, 1.0f);
        ALLEGRO_COLOR fadeColor = al_map_rgba_f(0, 0, 0, alpha);
        al_draw_filled_rectangle(0, 0, al_get_display_width(al_get_current_display()), al_get_display_height(al_get_current_display()), fadeColor);
    }
}