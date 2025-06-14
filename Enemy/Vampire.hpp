#ifndef Vampire_HPP
#define Vampire_HPP

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class Vampire : public Enemy
{
public:
    Vampire(float x, float y);
    virtual void Update(float deltaTime) override;
};

#endif // Vampire_HPP
