#ifndef AREAEFFECT_HPP
#define AREAEFFECT_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

class PlayScene;

class AreaEffect : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();

    void Update(float deltaTime) override;
    void Draw() const override;


public:
    AreaEffect(float x, float y, float maxRadius = 96.0f, float duration = 0.5f, ALLEGRO_COLOR color = al_map_rgb(255, 0, 0));
    float _time     = 0.0f;
    float _duration;
    float _maxRadius;
    ALLEGRO_COLOR _color;
};
#endif   // AREAEFFECT_HPP
