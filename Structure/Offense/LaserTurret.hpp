#ifndef LASERTURRET_HPP
#define LASERTURRET_HPP
#include "Tower.hpp"

class LaserTurret : public Tower {
public:
    static const int Price;
    LaserTurret(float x, float y);
    void CreateProjectile() override;
};
#endif   // LASERTURRET_HPP
