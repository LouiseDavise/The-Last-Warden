#ifndef SPLASHSCENE_HPP
#define SPLASHSCENE_HPP

#include "Engine/IScene.hpp"
#include <allegro5/allegro5.h>

class SplashScene final : public Engine::IScene {
private:
    float timer = 0.0f;
    ALLEGRO_BITMAP* logoBitmap = nullptr;
    float logoAlpha = 1.0f;
    float logoScale = 1.0f;

public:
    SplashScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif // SPLASHSCENE_HPP
