#ifndef LaserBullet_HPP
#define LaserBullet_HPP
#include "Projectile.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class LaserBullet : public Projectile {
public:
    explicit LaserBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // LaserBullet_HPP
