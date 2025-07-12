#ifndef STONEGOLEM_HPP
#define STONEGOLEM_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class StoneGolem : public Enemy
{
public:
    StoneGolem(float x, float y);
    virtual void Update(float deltaTime) override;
    void Hit(float damage) override;
};

#endif // STONEGOLEM_HPP
