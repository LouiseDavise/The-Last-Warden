#ifndef EXTREMETANKENEMY_HPP
#define EXTREMETANKENEMY_HPP
#include "Enemy.hpp"

class ExtremeTankEnemy : public Enemy
{
public:
    bool shieldActive = true;
    float shieldToggleTimer = 0.0f;
    float shieldToggleInterval = 1.0f;
    float damageReductionRatio = 1.0f;
    Engine::Sprite armorOverlay;
    ExtremeTankEnemy(int x, int y);
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
    bool IsTargetable() const override;
};
#endif // EXTREMETANKENEMY_HPP
