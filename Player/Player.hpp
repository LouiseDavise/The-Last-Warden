#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"
#include <allegro5/allegro.h>

class Player final : public Engine::Sprite {
public:
    float moveSpeed;
    Player(float x, float y);
    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);

private:
    bool keyW, keyA, keyS, keyD;
};

#endif // PLAYER_HPP