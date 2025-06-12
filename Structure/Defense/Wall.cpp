#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Wall.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"

Wall::Wall(std::string img, float x, float y, int price, float hp, float MAXhp, float radius)
    : Structure(img, x, y, price, hp, MAXhp, radius) {
    type = StructureType::Defense;
}

PlayScene* Wall::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

void Wall::Hit(float damage) {
    hp -= damage;
    if (hp <= 0) {
        getPlayScene()->mapState[occupyY][occupyX] = getPlayScene()->getTileType(0);
        getPlayScene()->StructureGroup->RemoveObject(objectIterator);
    }
}

void Wall::Update(float deltaTime) {
    Sprite::Update(deltaTime);
}

void Wall::Draw() const {
    Sprite::Draw();
    if (hp > 0) {
        const float barW = 40;
        const float barH = 6;
        const float yOff = CollisionRadius;
        const float left = Position.x - barW / 2;
        const float right = Position.x + barW / 2;
        const float top = Position.y - yOff;
        const float fillW = barW * (hp / MAXhp);

        al_draw_filled_rectangle(left, top, right, top + barH, al_map_rgb(40, 40, 40));

        float ratio = hp / MAXhp;
        ALLEGRO_COLOR color = al_map_rgb(100 + 50 * (1 - ratio), 150 + 50 * (1 - ratio), 255);  // Blue tone
        al_draw_filled_rectangle(left, top, left + fillW, top + barH, color);

        al_draw_rectangle(left, top, right, top + barH, al_map_rgb(255, 255, 255), 1);
    }
    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(50, 50, 255), 2);
    }
}
