#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include "Engine/Sprite.hpp"

class Structure : public Engine::Sprite {
protected:
    int price;
public:
    Structure(std::string img, float x, float y, int price);
    virtual ~Structure() = default;
    virtual bool IsShovel() const { return false; } // Optional, can override in derived classes
    int GetPrice() const {return price;}
    bool Preview = false;
    bool Enabled = true;
};

#endif // STRUCTURE_HPP
