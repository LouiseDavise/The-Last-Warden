#ifndef AXE_HPP
#define AXE_HPP
#include "Structure/Defense/Wall.hpp"

class Axe : public Wall {
public:
    static const int Price;
    Axe(float x, float y);
    bool IsAxe() const override { return true; }
};
#endif   // AXE_HPP
