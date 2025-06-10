#ifndef ARROW_HPP
#define ARROW_HPP
#include "Projectile.hpp"

class Enemy;
class Tower;
namespace Engine {
    struct Point;
}   // namespace Engine

class Arrow : public Projectile {
public:
    explicit Arrow(Engine::Point position, Engine::Point forwardDirection, float rotation, Tower *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // ARROW_HPP
