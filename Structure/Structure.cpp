#include "Structure.hpp"

Structure::Structure(std::string img, float x, float y, int price)
    : Engine::Sprite(img, x, y), price(price) {
}
