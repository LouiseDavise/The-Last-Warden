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

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    // Use existing audio
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // Title
    AddNewObject(new Engine::Label("WELCOME TO THE LAST WARDEN", "RealwoodRegular.otf", 90, halfW, halfH - 250, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Please select your option", "RealwoodRegular.otf", 45, halfW, halfH - 170, 160, 160, 160, 255, 0.5, 0.5));

    int newBtnW = 480;
    int newBtnH = 115;
    int newBtnX = (w - newBtnW) / 2;
    int newBtnY = halfH - 80;

    auto *btnNew = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", newBtnX, newBtnY - 5, newBtnW, newBtnH);
    btnNew->SetOnClickCallback(std::bind(&AuthScene::OnNewPlayerClick, this, 0));
    AddNewControlObject(btnNew);

    AddNewObject(new Engine::Label("NEW PLAYER", "RealwoodRegular.otf", 56, halfW, newBtnY + newBtnH / 2, 255, 255, 255, 255, 0.5, 0.5));

    int oldBtnW = 480;
    int oldBtnH = 115;
    int oldBtnX = (w - oldBtnW) / 2;
    int oldBtnY = halfH + 70;

    auto *btnOld = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", oldBtnX, oldBtnY - 5, oldBtnW, oldBtnH);
    btnOld->SetOnClickCallback(std::bind(&AuthScene::OnOldPlayerClick, this, 0));
    AddNewControlObject(btnOld);

    AddNewObject(new Engine::Label("OLD PLAYER", "RealwoodRegular.otf", 56, halfW, oldBtnY + oldBtnH / 2, 255, 255, 255, 255, 0.5, 0.5));
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
