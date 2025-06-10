#ifndef MACHINEGUNTURRET_HPP
#define MACHINEGUNTURRET_HPP
#include "Tower.hpp"

class MachineGunTurret : public Tower {
public:
    static const int Price;
    MachineGunTurret(float x, float y);
    void CreateProjectile() override;
};
#endif   // MACHINEGUNTURRET_HPP
