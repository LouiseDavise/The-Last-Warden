#ifndef LAVASLIME_HPP
#define LAVASLIME_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class LavaSlime : public Enemy
{
public:
    LavaSlime(float x, float y);
    virtual void Update(float deltaTime) override;
    void AOEAttack();
};

#endif // LAVASLIME_HPP
