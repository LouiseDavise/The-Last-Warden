#include "BasicWall.hpp"

const int BasicWall::Price = 20;
BasicWall::BasicWall(float x, float y)
    : Wall("Structures/BasicWall.png", x, y, Price, 200, 200, 32) {
}
