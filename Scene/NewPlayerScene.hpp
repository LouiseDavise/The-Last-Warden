#ifndef NEWPLAYER_SCENE_HPP
#define NEWPLAYER_SCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class NewPlayerScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit NewPlayerScene() = default;
    void Initialize() override;
    void Terminate() override;
    void OnNextClick(int stage);
    void OnBackClick(int stage);
    void OnKeyDown(int keyCode) override;
    void Draw() const override;
};

#endif // NEWPLAYER_SCENE_HPP
