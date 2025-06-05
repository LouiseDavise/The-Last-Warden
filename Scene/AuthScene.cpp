#include <functional>
#include "AuthScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void AuthScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // Use existing audio
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // Title
    AddNewObject(new Engine::Label("WELCOME TO TOWEL DEPEN", "pirulen.ttf", 48, halfW, halfH - 250, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Please select your option", "pirulen.ttf", 32, halfW, halfH - 190, 160, 160, 160, 255, 0.5, 0.5));

    // NEW PLAYER Button
    auto *btnNew = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 200, halfH - 50, 400, 100);
    btnNew->SetOnClickCallback(std::bind(&AuthScene::OnNewPlayerClick, this, 0));
    AddNewControlObject(btnNew);
    AddNewObject(new Engine::Label("NEW PLAYER", "pirulen.ttf", 36, halfW, halfH + 0, 66, 76, 110, 255, 0.5, 0.5));

    // OLD PLAYER Button
    auto *btnOld = new Engine::ImageButton("Tileset/dirt.png", "Tileset/floor.png", halfW - 200, halfH + 90, 400, 100);
    btnOld->SetOnClickCallback(std::bind(&AuthScene::OnOldPlayerClick, this, 0));
    AddNewControlObject(btnOld);
    AddNewObject(new Engine::Label("OLD PLAYER", "pirulen.ttf", 36, halfW, halfH + 140, 66, 76, 110, 255, 0.5, 0.5));
}

void AuthScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void AuthScene::OnNewPlayerClick(int)
{
    Engine::GameEngine::GetInstance().ChangeScene("new-player-scene");
}

void AuthScene::OnOldPlayerClick(int)
{
    Engine::GameEngine::GetInstance().ChangeScene("old-player-scene");
}
