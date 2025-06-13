#include "BasicWall.hpp"

const int BasicWall::Price = 10;
BasicWall::BasicWall(float x, float y)
    : Wall("Structures/BasicWall.png", x, y, Price, 100, 100, 32) {
}
