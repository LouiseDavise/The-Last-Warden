#ifndef BASICWALL_HPP
#define BASICWALL_HPP

#include "Structure/Defense/Wall.hpp"

class BasicWall : public Wall {
public:
    static const int Price;
    BasicWall(float x, float y);
    std::vector<std::string> GetInfoLines() const override {
    return {
        "Basic Wall",
        "HP     : 150",
        "Price  : 20"
    };
}
};

#endif // BASICWALL_HPP
