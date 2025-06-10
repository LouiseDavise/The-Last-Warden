#ifndef FireBullet_HPP
#define FireBullet_HPP
#include "Projectile.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class FireBullet : public Projectile {
public:
    explicit FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // FireBullet_HPP
