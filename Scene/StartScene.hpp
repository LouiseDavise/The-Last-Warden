#ifndef STARTSCENE_HPP
#define STARTSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <iostream>
#include <memory>

#include "Engine/IScene.hpp"
class StartScene final : public Engine::IScene
{
public:
    explicit StartScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void LeaderBoardOnClick(int stage);
    void SettingsOnClick(int stage);
};
#endif // STARTSCENE.HPP
