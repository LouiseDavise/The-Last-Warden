#include "SelectCompanionScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/AudioHelper.hpp"
#include "player_data.h"
#include <cstring>

void SelectCompanionScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    AddNewObject(new Engine::Label("CHOOSE YOUR COMPANION", "RealwoodRegular.otf", 72, halfW, 250, 255, 255, 255, 255, 0.5, 0.5));

    const int count = 3;
    const int spacing = 500;
    const int startX = halfW - spacing - 70;
    const int compY = 400;

    std::string compFolders[] = {"Support-1", "Support-2", "Support-3"};
    std::string compNames[] = {"Wisp", "Mochi", "Zuko"};

    for (int i = 0; i < count; ++i)
    {
        int x = startX + i * spacing;

        AddNewObject(new Engine::Image("Characters/" + compFolders[i] + "/image1x1.png", x, compY, 150, 150));
        AddNewObject(new Engine::Label(compNames[i], "pirulen.ttf", 28, x + 75, compY + 160, 255, 255, 255, 255, 0.5, 0.0));
        AddNewObject(new Engine::Label("Unique Style!", "pirulen.ttf", 12, x + 75, compY + 200, 200, 200, 200, 255, 0.5, 0.0));

        auto *btn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", x + 10, compY + 250, 130, 60);
        btn->SetOnClickCallback(std::bind(&SelectCompanionScene::OnSelectClick, this, i));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Select", "RealwoodRegular.otf", 28, x + 75, compY + 280, 255, 255, 255, 255, 0.5, 0.5));
    }

    // Confirm Box
    confirmBox = new Engine::Image("UI/confirm-box.png", halfW - 300, 350, 600, 400);
    confirmBox->Visible = false;
    AddNewObject(confirmBox);

    confirmText = new Engine::Label("", "RealwoodRegular.otf", 38, halfW, 460, 255, 255, 255, 255, 0.5, 0.5);
    confirmText->Visible = false;
    AddNewObject(confirmText);

    warningText1 = new Engine::Label("Prepare for battle alongside", "RealwoodRegular.otf", 26, halfW, 525, 255, 100, 100, 255, 0.5, 0.5);
    warningText1->Visible = false;
    AddNewObject(warningText1);
    warningText2 = new Engine::Label("", "RealwoodRegular.otf", 26, halfW, 550, 255, 100, 100, 255, 0.5, 0.5);
    warningText2->Visible = false;
    AddNewObject(warningText2);

    confirmBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW - 160, 600, 140, 65);
    confirmBtn->SetOnClickCallback([this]()
                                   {
        const char* types[] = {"COMP1", "COMP2", "COMP3"};
        strncpy(companionType, types[pendingCompanionId], sizeof(companionType));
        companionType[sizeof(companionType) - 1] = '\0';
        Engine::GameEngine::GetInstance().ChangeScene("play"); });
    confirmBtn->Visible = false;
    AddNewControlObject(confirmBtn);
    confirmLabel = new Engine::Label("Confirm", "pirulen.ttf", 17, halfW - 90, 630, 255, 255, 255, 255, 0.5, 0.5);
    confirmLabel->Visible = false;
    AddNewObject(confirmLabel);

    cancelBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW + 40, 600, 140, 65);
    cancelBtn->SetOnClickCallback([this]()
                                  {
        confirmBox->Visible = false;
        confirmText->Visible = false;
        warningText1->Visible = false;
        warningText2->Visible = false;
        confirmBtn->Visible = false;
        cancelBtn->Visible = false;
        confirmLabel->Visible = false;
        cancelLabel->Visible = false; });
    cancelBtn->Visible = false;
    AddNewControlObject(cancelBtn);
    cancelLabel = new Engine::Label("Cancel", "pirulen.ttf", 17, halfW + 110, 630, 255, 255, 255, 255, 0.5, 0.5);
    cancelLabel->Visible = false;
    AddNewObject(cancelLabel);
}

void SelectCompanionScene::OnSelectClick(int compId)
{
    pendingCompanionId = compId;
    const char *names[] = {"WISP", "MOCHI", "ZUKO"};
    confirmText->Text = "Choose " + std::string(names[compId]) + "?";
    warningText2->Text = std::string(names[compId]) + " THE FEARLESS!";
    confirmBox->Visible = true;
    confirmText->Visible = true;
    warningText1->Visible = true;
    warningText2->Visible = true;
    confirmBtn->Visible = true;
    cancelBtn->Visible = true;
    confirmLabel->Visible = true;
    cancelLabel->Visible = true;
}

void SelectCompanionScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
