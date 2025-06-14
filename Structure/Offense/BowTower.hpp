#ifndef BOWTOWER_HPP
#define BOWTOWER_HPP
#include "Tower.hpp"

class BowTower : public Tower {
public:
    static const int Price;
    BowTower(float x, float y);
    void CreateProjectile() override;
    std::vector<std::string> GetInfoLines() const override {
        return {
            "Bow Tower",
            "HP     : 70",
            "Price  : 70",
            "Range  : 350",
            "Damage : 10",
            "Speed  : 0.5 s"
        };  
    }
};
#endif   // BOWTOWER_HPP
