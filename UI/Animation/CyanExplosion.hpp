#ifndef CyanExplosion_HPP
#define CyanExplosion_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"

class PlayScene;

class CyanExplosion : public Engine::Sprite
{
protected:
    PlayScene *getPlayScene();
    float timeTicks;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    float timeSpan = 0.5;

public:
    CyanExplosion(float x, float y);
    void Update(float deltaTime) override;
};
#endif // CyanExplosion_HPP
