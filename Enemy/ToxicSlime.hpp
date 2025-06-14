#ifndef TOXICSLIME_HPP
#define TOXICSLIME_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class ToxicSlime : public Enemy
{
public:
    ToxicSlime(float x, float y);
    void Hit(float damage) override;
    bool toxicSpawned = false;
};

#endif // TOXICSLIME_HPP
