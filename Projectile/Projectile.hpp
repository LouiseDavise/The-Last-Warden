#ifndef Projectile_HPP
#define Projectile_HPP
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;
class Tower;
namespace Engine {
    struct Point;
}   // namespace Engine

class Projectile : public Engine::Sprite {
protected:
    float speed;
    float damage;
    Tower *parent;
    PlayScene *getPlayScene();
    virtual void OnExplode(Enemy *enemy);

public:
    Enemy *Target = nullptr;
    explicit Projectile(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Tower *parent);
    void Update(float deltaTime) override;
};
#endif   // Projectile_HPP
