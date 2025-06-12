#ifndef ORC_HPP
#define ORC_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class Orc : public Enemy
{
public:
    Orc(float x, float y);
};

#endif // ORC_HPP
