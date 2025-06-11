#include "Structure.hpp"

Structure::Structure(std::string img, float x, float y, int price, float hp, float MAXhp, float radius)
    : Engine::Sprite(img, x, y), price(price), hp(hp), MAXhp(MAXhp) {
    CollisionRadius = radius;
}

void Structure::Hit(float damage){}