#ifndef Vampire_HPP
#define Vampire_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Projectile/EnemyCircular.hpp"

class Vampire : public Enemy
{
public:
    Vampire(float x, float y);
    virtual void Update(float deltaTime) override;
        
    float attackCooldown = 1.5f;      
    float attackCooldownTimer = 0.0f;
    float attackRange = 700.0f;
};

#endif // Vampire_HPP
