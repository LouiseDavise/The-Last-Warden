#ifndef SHOVEL_HPP
#define SHOVEL_HPP
#include "Offense/Tower.hpp"

class Shovel : public Tower {
public:
    static const int Price;
    Shovel(float x, float y);
    bool IsShovel() const override { return true; }
    void CreateProjectile() override {}
};
#endif   // SHOVEL_HPP
