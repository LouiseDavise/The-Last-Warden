#ifndef BONFIRE_HPP
#define BONFIRE_HPP

#include "Structure/Defense/Wall.hpp"

class Bonfire : public Wall {
public:
    static const int Price;
    Bonfire(float x, float y);
};

#endif // BONFIRE_HPP
