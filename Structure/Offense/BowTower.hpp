#ifndef BOWTOWER_HPP
#define BOWTOWER_HPP
#include "Tower.hpp"

class BowTower : public Tower {
public:
    static const int Price;
    BowTower(float x, float y);
    void CreateProjectile() override;
};
#endif   // BOWTOWER_HPP
