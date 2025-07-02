#pragma once
#include "Projectile.hpp"

class MageBullet : public Projectile {
public:
    MageBullet(float x, float y, float damage, const Engine::Point& direction, float rotation);
    void Update(float deltaTime) override;
    void OnExplode();

private:
    Engine::Point velocity;
    float flightDist = 0.0f;
    float maxDist = 600.0f;
};
