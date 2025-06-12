#ifndef SMASHBONE_HPP
#define SMASHBONE_HPP
#include "Structure/Defense/Wall.hpp"

class SmashBone : public Wall {
public:
    static const int Price;
    SmashBone(float x, float y);
    bool IsSmashBone() const override { return true; }
};
#endif   // SMASHBONE_HPP
