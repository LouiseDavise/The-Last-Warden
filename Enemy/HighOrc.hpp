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
    virtual void Update(float deltaTime) override;

protected:
    float chargeRange = 700.0f;
    float chargeCooldown = 3.0f;
    float chargeDuration = 1.2f; 
    float chargeCooldownTimer = 0;
    float chargeDurationTimer = 0;
    float normalSpeed = 50;
    float chargeSpeed = 800;

    float blinkCooldownTimer = 0;
    float blinkCooldown = 0.85f;
    Engine::Point chargeTargetPos;
};

#endif // HIGHORC_HPP
