#ifndef SLIMEENEMY_HPP
#define SLIMEENEMY_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class SlimeEnemy : public Enemy
{
protected:
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> animationFrames;
    float animationTimer;
    float animationInterval;
    int currentFrame;

public:
    SlimeEnemy(float x, float y);
    void Update(float deltaTime) override;
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
};

#endif // SLIMEENEMY_HPP
