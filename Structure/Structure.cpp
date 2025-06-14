#include "Structure.hpp"

Structure::Structure(std::string img, float x, float y, int price, float hp, float MAXhp, float radius)
    : Engine::Sprite(img, x, y), price(price), hp(hp), MAXhp(MAXhp) {
    CollisionRadius = radius;
    // infoPanel = new Engine::Panel(Position.x + 40, Position.y - 80, 220, 200,
    //                               al_map_rgba(0, 0, 0, 160), 0, 0, GetInfoTitle());
    // infoPanel->Visible = false;
}

void Structure::Hit(float damage){}
