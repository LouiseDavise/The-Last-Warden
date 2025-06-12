#pragma once
#include "Projectile.hpp"

class MagicBullet : public Projectile {
public:
    MagicBullet(float x, float y, float damage, const Engine::Point& direction, float rotation);
};
