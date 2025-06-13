#include "Bonfire.hpp"

const int Bonfire::Price = 5;
Bonfire::Bonfire(float x, float y)
    : Wall("Structures/Bonfire.png", x, y, Price, 100, 100, 32) {
        isLightSource = true;
}
