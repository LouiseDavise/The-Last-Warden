#include "SelectCompanionScene.hpp"
#include <allegro5/allegro_primitives.h>
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/AudioHelper.hpp"
#include "player_data.h"
#include <cstring>

void SelectCompanionScene::Initialize()
{
    fadingToPlay = false;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    AddNewObject(new Engine::Label("CHOOSE YOUR COMPANION", "RealwoodRegular.otf", 72, halfW, 250, 255, 255, 255, 255, 0.5, 0.5));

    const int count = 3;
    const int spacing = 500;
    const int startX = halfW - spacing - 70;
    const int compY = 400;

    struct CompanionOption
    {
        std::string name;
        std::string desc1;
        std::string desc2;
        std::string imagePath;
    };

    CompanionOption companions[count] = {
        {"Wisp", "Swaps place with player", "", "Characters/Support-1/image1x1.png"},
        {"Mochi", "Heals player nearby & unlimited", "attack within range", "Characters/Support-2/image1x1.png"},
        {"Zuko", "Knocks back enemies", "", "Characters/Support-3/image1x1.png"}};

    for (int i = 0; i < count; ++i)
    {
        int x = startX + i * spacing;

        // Image
        AddNewObject(new Engine::Image(companions[i].imagePath, x, compY, 150, 150));

        // Companion name
        AddNewObject(new Engine::Label(companions[i].name, "pirulen.ttf", 28, x + 75, compY + 160, 255, 255, 255, 255, 0.5, 0.0));

        // Description lines
        AddNewObject(new Engine::Label(companions[i].desc1, "pirulen.ttf", 12, x + 75, compY + 200, 200, 200, 200, 255, 0.5, 0.0));
        if (!companions[i].desc2.empty())
            AddNewObject(new Engine::Label(companions[i].desc2, "pirulen.ttf", 12, x + 75, compY + 225, 200, 200, 200, 255, 0.5, 0.0));

        // Select button
        auto *btn = new Engine::ImageButton("UI/button.png", "UI/button.png", x + 10, compY + 250, 130, 60);
        btn->SetOnClickCallback(std::bind(&SelectCompanionScene::OnSelectClick, this, i));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Select", "RealwoodRegular.otf", 28, x + 75, compY + 280, 255, 255, 255, 255, 0.5, 0.5));
    }

    // Confirmation box
    confirmBox = new Engine::Image("UI/confirm-box.png", halfW - 300, 350, 600, 400);
    confirmBox->Visible = false;
    AddNewObject(confirmBox);

    // Confirmation text
    confirmText = new Engine::Label("", "RealwoodRegular.otf", 28, halfW, 460, 255, 255, 255, 255, 0.5, 0.5);
    confirmText->Visible = false;
    AddNewObject(confirmText);

    // Warning text
    warningText1 = new Engine::Label("Prepare for battle alongside", "RealwoodRegular.otf", 20, halfW, 525, 255, 100, 100, 255, 0.5, 0.5);
    warningText1->Visible = false;
    AddNewObject(warningText1);

    warningText2 = new Engine::Label("", "RealwoodRegular.otf", 20, halfW, 545, 255, 100, 100, 255, 0.5, 0.5);
    warningText2->Visible = false;
    AddNewObject(warningText2);

    // Confirm button
    confirmBtn = new Engine::ImageButton("UI/button.png", "UI/button.png", halfW - 160, 600, 140, 65);
    confirmBtn->SetOnClickCallback([this]()
                                   {
    const char* types[] = {"COMP1", "COMP2", "COMP3"};
    strncpy(companionType, types[pendingCompanionId], sizeof(companionType));
    companionType[sizeof(companionType) - 1] = '\0';
    auto &engine = Engine::GameEngine::GetInstance();
    if (engine.GlobalBGMInstance) {
        AudioHelper::StopSample(engine.GlobalBGMInstance);
        engine.GlobalBGMInstance = nullptr;
    }
    fadingToPlay = true;
    fadeTimer = 0; });

    confirmBtn->Visible = false;
    AddNewControlObject(confirmBtn);

    confirmLabel = new Engine::Label("Confirm", "pirulen.ttf", 17, halfW - 90, 630, 255, 255, 255, 255, 0.5, 0.5);
    confirmLabel->Visible = false;
    AddNewObject(confirmLabel);

    // Cancel button
    cancelBtn = new Engine::ImageButton("UI/button.png", "UI/button.png", halfW + 40, 600, 140, 65);
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

    // Back Button
    auto *backButton = new Engine::ImageButton("UI/button.png", "UI/button.png", halfW - 240, 790, 480, 115);
    backButton->SetOnClickCallback([]()
                                   { Engine::GameEngine::GetInstance().ChangeScene("mode-select"); });
    AddNewControlObject(backButton);
    AddNewObject(new Engine::Label("BACK", "RealwoodRegular.otf", 56, halfW, 847, 255, 255, 255, 255, 0.5, 0.5));
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

void SelectCompanionScene::Update(float deltaTime) {
    if (fadingToPlay) {
        fadeTimer += deltaTime;
        if (fadeTimer >= fadeDuration) {
            fadeTimer = fadeDuration;
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }
    }
    IScene::Update(deltaTime);
}

void SelectCompanionScene::Draw() const {
    IScene::Draw();
    if (fadingToPlay) {
        float alpha = std::min(fadeTimer / fadeDuration, 1.0f);
        ALLEGRO_COLOR fadeColor = al_map_rgba_f(0, 0, 0, alpha);
        al_draw_filled_rectangle(
            0, 0,
            al_get_display_width(al_get_current_display()),
            al_get_display_height(al_get_current_display()),
            fadeColor
        );
    }
}

void SelectCompanionScene::Terminate()
{
    IScene::Terminate();
}
