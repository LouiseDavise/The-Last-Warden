#ifndef STRUCTUREBUTTON_HPP
#define STRUCTUREBUTTON_HPP
#include <string>

#include "Engine/Sprite.hpp"
#include "UI/Component/ImageButton.hpp"

class PlayScene;

class StructureButton : public Engine::ImageButton {
protected:
    PlayScene *getPlayScene();

public:
    int money;
    Engine::Sprite Base;
    Engine::Sprite Tower;
    StructureButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Tower, float x, float y, int money);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // STRUCTUREBUTTON_HPP
