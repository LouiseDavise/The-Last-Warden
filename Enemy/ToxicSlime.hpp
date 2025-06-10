#ifndef TOXICSLIME_HPP
#define TOXICSLIME_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class ToxicSlime : public Enemy
{
protected:
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> animationFrames;
    float animationTimer;
    float animationInterval;
    int currentFrame;

public:
    ToxicSlime(float x, float y);
    void Update(float deltaTime) override;
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
};

#endif // TOXICSLIME_HPP
