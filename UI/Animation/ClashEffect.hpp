#ifndef ClashEffect_HPP
#define ClashEffect_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"

class PlayScene;

class ClashEffect : public Engine::Sprite
{
protected:
    PlayScene *getPlayScene();
    float timeTicks;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    float timeSpan = 0.5;

public:
    ClashEffect(float x, float y);
    void Update(float deltaTime) override;
};
#endif // ClashEffect_HPP
