#include <functional>
#include "AuthScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "player_data.h"
#include <cstdlib>
#include <ctime>

char nameInput[17] = "PLAYER";
int nameInputLen = 6;
char heroType[10] = "";
char player_uid[20] = "";
char companionType[10] = "";
bool cameFromScoreScene = false;
char current_bgm[32] = "";

void AuthScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    // audio
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int randomIndex = std::rand() % 6 + 1;

    std::string filename = "scene-" + std::to_string(randomIndex) + ".ogg";

    std::strncpy(current_bgm, filename.c_str(), sizeof(current_bgm));
    current_bgm[sizeof(current_bgm) - 1] = '\0'; // null-terminate

    Engine::GameEngine::GetInstance().GlobalBGMInstance =
        AudioHelper::PlaySample(filename, true, AudioHelper::BGMVolume);

    // Title
    AddNewObject(new Engine::Label("WELCOME TO THE LAST WARDEN", "RealwoodRegular.otf", 90, halfW, halfH - 250, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Please select your option", "RealwoodRegular.otf", 45, halfW, halfH - 170, 160, 160, 160, 255, 0.5, 0.5));

    int newBtnW = 480;
    int newBtnH = 115;
    int newBtnX = (w - newBtnW) / 2;
    int newBtnY = halfH - 80;

    auto *btnNew = new Engine::ImageButton("UI/button.png", "UI/button.png", newBtnX, newBtnY - 5, newBtnW, newBtnH);
    btnNew->SetOnClickCallback(std::bind(&AuthScene::OnLoginClick, this));
    AddNewControlObject(btnNew);

    AddNewObject(new Engine::Label("LOGIN", "RealwoodRegular.otf", 56, halfW, newBtnY + newBtnH / 2, 255, 255, 255, 255, 0.5, 0.5));

    int oldBtnW = 480;
    int oldBtnH = 115;
    int oldBtnX = (w - oldBtnW) / 2;
    int oldBtnY = halfH + 70;

    auto *btnOld = new Engine::ImageButton("UI/button.png", "UI/button.png", oldBtnX, oldBtnY - 5, oldBtnW, oldBtnH);
    btnOld->SetOnClickCallback(std::bind(&AuthScene::OnRegisterClick, this));
    AddNewControlObject(btnOld);

    AddNewObject(new Engine::Label("REGISTER", "RealwoodRegular.otf", 56, halfW, oldBtnY + oldBtnH / 2, 255, 255, 255, 255, 0.5, 0.5));
}

void AuthScene::Terminate()
{
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void AuthScene::OnRegisterClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("register-scene");
}

void AuthScene::OnLoginClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("login-scene");
}
