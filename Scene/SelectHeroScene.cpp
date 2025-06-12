#include <allegro5/allegro_audio.h>
#include <functional>
#include <string>
#include "SelectHeroScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/AudioHelper.hpp"
#include "player_data.h"

#include <fstream>
#include <cstring>

void save_player_uid_and_name(const std::string &uid, const std::string &player_name, const std::string &hero_type)
{
    std::filesystem::create_directories("Data");

    std::ofstream file("Data/player_uid.txt", std::ios::app);
    std::cout << "Writing to: " << std::filesystem::absolute("Data/player_uid.txt") << std::endl;

    if (file.is_open())
    {
        file << uid << "," << player_name << "," << hero_type << "\n";
        file.close();
        std::cout << "Player UID, name, and hero type appended to player_uid.txt\n";
    }
    else
    {
        std::cout << "Failed to write to file.\n";
    }
}

void SelectHeroScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    AddNewObject(new Engine::Label("CHOOSE YOUR HERO", "RealwoodRegular.otf", 72, halfW, 250, 255, 255, 255, 255, 0.5, 0.5));

    const int charCount = 3;
    const int spacing = 500;
    const int startX = halfW - spacing - 70;
    const int charY = 400;

    struct HeroOption
    {
        std::string name;
        std::string description;
        std::string imagePath;
    };

    HeroOption heroes[charCount] = {
        {"Spearman", "A primitive but strong melee brawler.", "Characters/Spearman/image1x1.png"},
        {"Archer", "Quick ranged attacker with high DPS.", "Characters/Mage1x1.png"},
        {"Mage", "Master of elemental magic and AoE.", "Characters/Mage1x1.png"}};

    for (int i = 0; i < charCount; ++i)
    {
        int x = startX + i * spacing;

        // Image
        AddNewObject(new Engine::Image(heroes[i].imagePath, x, charY, 150, 150));

        // Hero name
        AddNewObject(new Engine::Label(heroes[i].name, "pirulen.ttf", 28, x + 75, charY + 160, 255, 255, 255, 255, 0.5, 0.0));

        // Description
        AddNewObject(new Engine::Label(heroes[i].description, "pirulen.ttf", 12, x + 75, charY + 200, 200, 200, 200, 255, 0.5, 0.0));

        // Choose button
        auto *btn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", x + 10, charY + 250, 130, 60);
        btn->SetOnClickCallback(std::bind(&SelectHeroScene::OnSelectClick, this, i));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Select", "RealwoodRegular.otf", 28, x + 75, charY + 280, 255, 255, 255, 255, 0.5, 0.5));
    }

    // Confirmation box
    confirmBox = new Engine::Image("UI/confirm-box.png", halfW - 300, 350, 600, 400); // make your own box image
    confirmBox->Visible = false;
    AddNewObject(confirmBox);

    // Confirmation text
    confirmText = new Engine::Label("", "RealwoodRegular.otf", 28, halfW, 460, 255, 255, 255, 255, 0.5, 0.5);
    confirmText->Visible = false;
    AddNewObject(confirmText);

    // Warning text (split into 2 lines)
    warningText1 = new Engine::Label("You can only choose your hero once.",
                                     "RealwoodRegular.otf", 20, halfW, 525, 255, 100, 100, 255, 0.5, 0.5);
    warningText1->Visible = false;
    AddNewObject(warningText1);

    warningText2 = new Engine::Label("This decision is final.",
                                     "RealwoodRegular.otf", 20, halfW, 545, 255, 100, 100, 255, 0.5, 0.5);
    warningText2->Visible = false;
    AddNewObject(warningText2);

    // Confirm button
    confirmBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW - 160, 600, 140, 65);
    confirmBtn->SetOnClickCallback([this]()
                                   {
    const char* types[] = {"SPEARMAN", "ARCHER", "MAGE"};
    strncpy(heroType, types[pendingHeroId], sizeof(heroType));
    heroType[sizeof(heroType) - 1] = '\0';

    save_player_uid_and_name(player_uid, nameInput, heroType);

    // Save to file: player_uid.txt

    // Proceed to next scene
    Engine::GameEngine::GetInstance().ChangeScene("mode-select"); });
    confirmBtn->Visible = false;
    AddNewControlObject(confirmBtn);
    confirmLabel = new Engine::Label("Confirm", "pirulen.ttf", 17, halfW - 90, 630, 255, 255, 255, 255, 0.5, 0.5);
    confirmLabel->Visible = false;
    AddNewObject(confirmLabel);

    // Cancel button
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

void SelectHeroScene::OnSelectClick(int heroId)
{
    pendingHeroId = heroId;
    const char *heroNames[] = {"Spearman", "Archer", "Mage"};

    std::string message = "Are you sure you want to pick the " + std::string(heroNames[heroId]) + " hero?";
    confirmText->Text = message;

    // Show popup
    confirmBox->Visible = true;
    confirmText->Visible = true;
    warningText1->Visible = true;
    warningText2->Visible = true;
    confirmBtn->Visible = true;
    cancelBtn->Visible = true;
    confirmLabel->Visible = true;
    cancelLabel->Visible = true;
}

void SelectHeroScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
