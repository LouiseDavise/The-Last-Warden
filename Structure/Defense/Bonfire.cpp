#include "Bonfire.hpp"

const int Bonfire::Price = 5;
Bonfire::Bonfire(float x, float y)
    : Wall("Structures/Bonfire.png", x, y, Price, 50, 50, 32) {
        isLightSource = true;
}
