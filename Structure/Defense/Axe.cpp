#include "Axe.hpp"

const int Axe::Price = 10;
Axe::Axe(float x, float y)
    : Wall("Structures/Axe.png", x, y, Price, 0, 0, 32) {
}
