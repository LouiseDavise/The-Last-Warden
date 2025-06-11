#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Wall.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"

Wall::Wall(std::string img, float x, float y, float hp, float radius)
    : Structure(img, x, y), hp(hp), MAXhp(hp) {
    CollisionRadius = radius;
}

void Wall::Hit(float damage) {
    hp -= damage;
    if (hp <= 0) {
        // Remove this wall from the scene
        getPlayScene()->DefenseGroup->RemoveObject(objectIterator);
    }
}

void Wall::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    // You may add blinking / damage effect / animation here in the future
    if (hp <= 0) {
        getPlayScene()->DefenseGroup->RemoveObject(objectIterator);
    }
}

void Wall::Draw() const {
    Sprite::Draw();

    if (hp > 0) {
        const float barW = 40;
        const float barH = 6;
        const float yOff = CollisionRadius + 10;
        const float left = Position.x - barW / 2;
        const float right = Position.x + barW / 2;
        const float top = Position.y - yOff;
        const float fillW = barW * (hp / MAXhp);

        // Background
        al_draw_filled_rectangle(left, top, right, top + barH, al_map_rgb(40, 40, 40));
        // Health fill (green→red)
        float ratio = hp / MAXhp;
        ALLEGRO_COLOR color = al_map_rgb(255 * (1 - ratio), 255 * ratio, 0);
        al_draw_filled_rectangle(left, top, left + fillW, top + barH, color);
        // White border
        al_draw_rectangle(left, top, right, top + barH, al_map_rgb(255, 255, 255), 1);
    }
}
