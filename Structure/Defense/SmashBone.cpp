#include "SmashBone.hpp"

const int SmashBone::Price = 0;
SmashBone::SmashBone(float x, float y)
    : Wall("Structures/SmashBone.png", x, y, Price, 0, 0, 32) {
}
