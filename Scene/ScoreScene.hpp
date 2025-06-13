#ifndef SCORESCENE_HPP
#define SCORESCENE_HPP
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

class ScoreScene final : public Engine::IScene {
private:
    Engine::Label* scoreLabel;
    Engine::Label* timeLabel;
    Engine::Label* killLabel;
    Engine::ImageButton* backButton;

public:
    explicit ScoreScene() = default;
    void Initialize() override;
    void Terminate() override;
};

#endif // SCORESCENE_HPP
