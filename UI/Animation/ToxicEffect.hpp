#ifndef TOXIC_EFFECT_HPP
#define TOXIC_EFFECT_HPP

#include <string>
#include <vector>
#include "Engine/IObject.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class ToxicEffect : public Engine::IObject {
private:
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> frames;
    float lifeSpan;
    float lifeElapsed;
    float elapsedTime;
    float frameDuration;
    int currentFrame;
    PlayScene* getPlayScene();
    float totalDuration;

public:
    ToxicEffect(float x, float y, float lifeSpan);
    ~ToxicEffect();
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif //TOXIC_EFFECT_HPP
