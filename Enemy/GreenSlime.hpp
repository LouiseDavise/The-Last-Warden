#ifndef GREENSLIME_HPP
#define GREENSLIME_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class GreenSlime : public Enemy
{
public:
    GreenSlime(float x, float y);
};

#endif // GREENSLIME_HPP
