#ifndef Bom_HPP
#define Bom_HPP

#include "Engine/Sprite.hpp"

class Bom : public Engine::Sprite
{
public:
    static const int Cost;
    bool activated = false;
    bool placed = false; 

    Bom(float x, float y);
    void Update(float deltaTime) override;
};

#endif // Bom_HPP
