#ifndef OLDPLAYER_SCENE_HPP
#define OLDPLAYER_SCENE_HPP

#include <memory>
#include <allegro5/allegro_audio.h>
#include "Engine/IScene.hpp"

class OldPlayerScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::string uidInput;
    bool switch_key = false;
    bool isInvalidUID = false;

public:
    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;

    void OnSubmitClick(int stage);
    void OnBackClick(int stage);

    static bool ValidateUID(const std::string& uid, std::string& outName, std::string& outHero);
};

#endif // OLDPLAYER_SCENE_HPP
