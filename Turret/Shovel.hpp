#ifndef SHOVEL_HPP
#define SHOVEL_HPP

#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

class PlayScene;

class Shovel : public Engine::Sprite {
public:
    static const int Cost;
    Shovel(float x, float y);

    void Update(float deltaTime) override;
    void OnClick(int gridX, int gridY);
};

#endif // SHOVEL_HPP
