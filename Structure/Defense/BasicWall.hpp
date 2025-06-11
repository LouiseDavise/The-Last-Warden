#ifndef BASICWALL_HPP
#define BASICWALL_HPP

#include "Structure/Defense/Wall.hpp"

class BasicWall : public Wall {
public:
    static const int Price;
    BasicWall(float x, float y);
};

#endif // BASICWALL_HPP
