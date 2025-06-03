#ifndef SUPERTANKENEMY_HPP
#define SUPERTANKENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class SuperTankEnemy : public Enemy
{
public:
    float damageReductionRatio = 0.5f;
    Engine::Sprite armorOverlay;
    SuperTankEnemy(int x, int y);
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif // SUPERTANKENEMY_HPP
