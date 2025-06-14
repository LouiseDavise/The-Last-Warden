#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include "Engine/Sprite.hpp"

enum class StructureType {
    Offense,
    Defense
};

class Structure : public Engine::Sprite {
protected:
    int price;
    float hp, MAXhp;
public:
    Structure(std::string img, float x, float y, int price, float hp, float MAXhp, float radius);
    virtual ~Structure() = default;
    virtual void Hit(float damage);
    virtual bool IsSmashBone() const { return false; }
    virtual bool IsAxe() const { return false; }
    int GetPrice() const {return price;}
    virtual std::vector<std::string> GetInfoLines() const {
        return {"Structure", "HP: ?", "Price: ?"};
    }
    bool Preview = false;
    bool Enabled = true;
    StructureType type;   
    float occupyX, occupyY;
};

#endif // STRUCTURE_HPP
