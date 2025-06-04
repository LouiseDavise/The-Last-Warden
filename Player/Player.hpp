#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"
#include <vector>
#include <memory>

class Player final : public Engine::Sprite {
public:
    float moveSpeed;
    Player(float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);

private:
    bool keyW, keyA, keyS, keyD;
    bool faceRight;
    float animationTimer;
    int currentFrame;
    float animationInterval;

    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> animationsRight;
    std::shared_ptr<ALLEGRO_BITMAP> currentBitmap;

    void UpdateAnimation(float deltaTime, bool movingHorizontally);
};

#endif
