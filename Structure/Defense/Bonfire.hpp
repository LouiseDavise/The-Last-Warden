#ifndef BONFIRE_HPP
#define BONFIRE_HPP

#include "Structure/Defense/Wall.hpp"

class Bonfire : public Wall {
public:
    static const int Price;
    Bonfire(float x, float y);
    std::vector<std::string> GetInfoLines() const override {
        return {
            "Bonfire",
            "HP     : 50" ,
            "Price  : 5",
            "Warmth : 130"
        };
    }
};

#endif // BONFIRE_HPP
