#pragma once
#include "Projectile.hpp"

class EnemyCircular : public Projectile
{
public:
    EnemyCircular(float x, float y, float damage, const Engine::Point &direction, float rotation, float maxDist);
    void Update(float dt) override;

private:
    Engine::Point velocity;
    float flightDist = 0.0f;
    float maxDist = 450.0f;
    float speed = 800.0f;
};
