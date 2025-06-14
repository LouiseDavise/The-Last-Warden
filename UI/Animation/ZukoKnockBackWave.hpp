#ifndef ZUKOKNOCKBACKWAVE_HPP
#define ZUKOKNOCKBACKWAVE_HPP
#include <allegro5/allegro.h>
#include <memory>
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"

class ZukoKnockbackWave : public Engine::Sprite
{
public:
    explicit ZukoKnockbackWave(float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;

private:
    float timer = 0;
    float duration = 0.5f; // short burst
    float maxRadius = 500; // very small range
    std::shared_ptr<ALLEGRO_BITMAP> waveBmp;
};

#endif // ZUKOKNOCKBACKWAVE_HPP