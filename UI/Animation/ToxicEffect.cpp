#include "ToxicEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Group.hpp"

ToxicEffect::ToxicEffect(float x, float y, float lifeSpan)
    : currentFrame(0), elapsedTime(0), frameDuration(0.05f), lifeSpan(lifeSpan), lifeElapsed(0){
    Position.x = x;
    Position.y = y;
    Anchor.x = 0.5f;
    Anchor.y = 0.5f;

    // Load all 16 frames
    for (int i = 1; i <= 16; ++i) {
        std::string path = "Effects/Toxic-" + std::to_string(i) + ".png";
        frames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    Size.x = 192;
    Size.y = 256;
}

PlayScene* ToxicEffect::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

void ToxicEffect::Update(float deltaTime) {
    lifeElapsed += deltaTime;
    if (lifeElapsed >= lifeSpan) {
        getPlayScene()->GroundEffectGroup->RemoveObject(objectIterator);
        return;
    }

    elapsedTime += deltaTime;

    int frameCount = frames.size();
    currentFrame = static_cast<int>(elapsedTime / frameDuration) % frameCount;
}

void ToxicEffect::Draw() const {
    if (currentFrame < 0 || currentFrame >= static_cast<int>(frames.size()))
        return;
    ALLEGRO_BITMAP* bmp = frames[currentFrame].get();
    al_draw_tinted_scaled_rotated_bitmap(
        bmp, al_map_rgba(255, 255, 255, 255),
        0, 0, // origin
        Position.x, Position.y,
        1.0f, 1.0f,
        0,
        0
    );
}

ToxicEffect::~ToxicEffect() {
}
