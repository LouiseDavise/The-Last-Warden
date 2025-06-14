#include "BasicWall.hpp"

const int BasicWall::Price = 20;
BasicWall::BasicWall(float x, float y)
    : Wall("Structures/BasicWall.png", x, y, Price, 150, 150, 32) {
}
