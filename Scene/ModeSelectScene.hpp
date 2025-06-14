#ifndef MODESELECTSCENE_HPP
#define MODESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class ModeSelectScene final : public Engine::IScene
{
public:
    explicit ModeSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void SettingsOnClick();
    void BackOnClick(int stage);
    bool stopBGMOnTerminate = false;
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
    void Update(float deltaTime) override;
    void Draw() const override;
    bool fadingToPlay = false;
    float fadeTimer = 0.0f;
    const float fadeDuration = 1.5f;
};

#endif // MODESELECTSCENE_HPP
