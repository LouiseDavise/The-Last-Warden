#ifndef LogoScene_HPP
#define LogoScene_HPP

#include "Engine/IScene.hpp"
#include <allegro5/allegro5.h>

class LogoScene final : public Engine::IScene {
private:
    float timer = 0.0f;
    ALLEGRO_BITMAP* logoBitmap = nullptr;
    float logoAlpha = 1.0f;
    float logoScale = 1.0f;

public:
    LogoScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif // LogoScene_HPP
