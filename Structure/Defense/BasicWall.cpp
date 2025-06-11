#include "BasicWall.hpp"

const int BasicWall::Price = 5;
BasicWall::BasicWall(float x, float y)
    : Wall("Structures/BasicWall.png", x, y, Price, 100, 100, 32) {
}
