#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "ModeSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void ModeSelectScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    btn = new Engine::ImageButton("UI/basic-button.png", "UI/basic-button-hover.png", halfW - 240, halfH / 2 - 50, 480, 115);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("1-PLAYER", "RealwoodRegular.otf", 56, halfW, halfH / 2 + 10, 28, 15, 0, 255, 0.5, 0.5));
    btn = new Engine::ImageButton("UI/basic-button.png", "UI/basic-button-hover.png", halfW - 240, halfH / 2 + 100, 480, 115);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("2-PLAYER", "RealwoodRegular.otf", 56, halfW, halfH / 2 + 160, 28, 15, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    btn = new Engine::ImageButton("UI/basic-button.png", "UI/basic-button-hover.png", halfW - 240, halfH / 2 + 350, 480, 115);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "RealwoodRegular.otf", 56, halfW, halfH / 2 + 410, 28, 15, 0, 255, 0.5, 0.5));
}

void ModeSelectScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void ModeSelectScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
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