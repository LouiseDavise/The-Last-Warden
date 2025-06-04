#include "Player.hpp"

Player::Player(float x, float y)
    : Engine::Sprite("play/enemy-4.png", x, y), moveSpeed(200.0f), keyW(false), keyA(false), keyS(false), keyD(false) {
    Anchor = Engine::Point(0.5f, 0.5f);
}

void Player::Update(float deltaTime) {  
    Engine::Point velocity(0, 0);
    if (keyW) velocity.y -= 1;
    if (keyS) velocity.y += 1;
    if (keyA) velocity.x -= 1;
    if (keyD) velocity.x += 1;
    if (velocity.x != 0 || velocity.y != 0) {
        float len = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        Position.x += (velocity.x / len) * moveSpeed * deltaTime;
        Position.y += (velocity.y / len) * moveSpeed * deltaTime;
    }
    Engine::Sprite::Update(deltaTime);
}

void Player::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_W) keyW = true;
    if (keyCode == ALLEGRO_KEY_A) keyA = true;
    if (keyCode == ALLEGRO_KEY_S) keyS = true;
    if (keyCode == ALLEGRO_KEY_D) keyD = true;
}

void Player::OnKeyUp(int keyCode) {
    if (keyCode == ALLEGRO_KEY_W) keyW = false;
    if (keyCode == ALLEGRO_KEY_A) keyA = false;
    if (keyCode == ALLEGRO_KEY_S) keyS = false;
    if (keyCode == ALLEGRO_KEY_D) keyD = false;
}