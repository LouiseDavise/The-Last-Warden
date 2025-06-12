#ifndef HIGHORC_HPP
#define HIGHORC_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class HighOrc : public Enemy
{
public:
    HighOrc(float x, float y);
};

#endif // HIGHORC_HPP
