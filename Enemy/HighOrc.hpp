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
    virtual void Draw() const override;
    Engine::Point chargeTargetPos;
    Engine::Point chargeOrigin;
    Engine::Point chargeDestination;

protected:
    float chargeRange = 600.0f;
    float stopRange = 900.0f;
    float chargeCooldown = 4.5f;
    float chargeCooldownTimer = 0;
    float normalSpeed = 60;
    float chargeSpeed = 600;

    float blinkCooldownTimer = 0;
    float blinkCooldown = 1.5f;
};

#endif // HIGHORC_HPP
