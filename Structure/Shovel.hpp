#ifndef SHOVEL_HPP
#define SHOVEL_HPP
#include "Defense/Turret.hpp"

class Shovel : public Turret {
public:
    static const int Price;
    Shovel(float x, float y);
    bool IsShovel() const override { return true; }
    void CreateBullet() override {}
};
#endif   // SHOVEL_HPP
