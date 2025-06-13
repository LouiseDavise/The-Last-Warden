#ifndef AUTHSCENE_HPP
#define AUTHSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class AuthScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit AuthScene() = default;
    void Initialize() override;
    void Terminate() override;

    void OnLoginClick();
    void OnRegisterClick();
};

#endif // AUTHSCENE_HPP
