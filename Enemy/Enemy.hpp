#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Projectile;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite
{
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float damage;
    int money;
    PlayScene *getPlayScene();

public:
    std::list<Turret *> lockedTurrets;
    std::list<Projectile *> lockedProjectiles;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, float damage, int money);
    virtual void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void Update(float deltaTime) override;
    void Draw() const override;
    virtual bool IsTargetable() const { return true; }

    float GetRadius() const { return CollisionRadius; }
    float GetDamage() const { return damage; }
};
#endif // ENEMY_HPP
