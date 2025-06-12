#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "SettingScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SettingScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("Backgrounds/01.png", 0, 0, w, h));

    Engine::ImageButton *btn;

    Slider *sliderBGM, *sliderSFX;
    int sliderWidth = 400;
    int sliderHeight = 6;
    int sliderX = halfW - sliderWidth / 2;
    int labelOffsetX = -80;

    AddNewObject(new Engine::Label("SETTINGS", "RealwoodRegular.otf", 72, halfW, halfH - 300, 255, 255, 255, 255, 0.5, 0.5));
    // BGM Slider
    sliderBGM = new Slider(sliderX, halfH - 150, sliderWidth, sliderHeight);
    sliderBGM->SetOnValueChangedCallback(std::bind(&SettingScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderBGM);
    AddNewObject(new Engine::Label("BGM:", "RealwoodRegular.otf", 40, sliderX - 120, halfH - 150 + sliderHeight / 2, 255, 255, 255, 255, 0.0, 0.5));

    // SFX Slider
    sliderSFX = new Slider(sliderX, halfH - 30, sliderWidth, sliderHeight);
    sliderSFX->SetOnValueChangedCallback(std::bind(&SettingScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderSFX);
    AddNewObject(new Engine::Label("SFX:", "RealwoodRegular.otf", 40, sliderX - 120, halfH - 30 + sliderHeight / 2, 255, 255, 255, 255, 0.0, 0.5));

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderSFX->SetValue(AudioHelper::SFXVolume);

    // Back Button
    btn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", halfW - 200, halfH / 2 + 420, 400, 100);
    btn->SetOnClickCallback(std::bind(&SettingScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "RealwoodRegular.otf", 56, halfW, halfH / 2 + 475, 255, 255, 255, 255, 0.5, 0.5));
}
void SettingScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void SettingScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}

void SettingScene::BGMSlideOnValueChanged(float value)
{
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void SettingScene::SFXSlideOnValueChanged(float value)
{
    AudioHelper::SFXVolume = value;
}