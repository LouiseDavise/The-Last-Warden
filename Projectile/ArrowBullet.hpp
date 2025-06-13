#pragma once
#include "Projectile.hpp"

class ArrowBullet : public Projectile {
public:
    ArrowBullet(float x, float y, float damage, const Engine::Point& direction, float rotation);
    void Update(float deltaTime) override;

private:
    Engine::Point velocity;
    float flightDist = 0.0f;
    float maxDist = 800.0f;
};
