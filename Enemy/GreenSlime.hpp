#ifndef GREENSLIME_HPP
#define GREENSLIME_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class GreenSlime : public Enemy
{
protected:
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> runFrames;
    float animationTimer;
    float animationInterval;
    int currentFrame;

public:
    bool faceRight;
    GreenSlime(float x, float y);
    void Update(float deltaTime) override;
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void Draw() const override;
};

#endif // GREENSLIME_HPP
