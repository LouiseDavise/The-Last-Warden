#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Projectile;
class PlayScene;
class Tower;

enum class State {
    Run,
    Dying
};

class Enemy : public Engine::Sprite
{
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float damage;
    int money;
    State state;

    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> runFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> deathFrames;
    float runTimer;
    float runInterval;
    float deathTimer;
    float deathInterval;
    int currentFrame;
    bool faceRight;

    PlayScene *getPlayScene();

public:
    std::list<Tower *> lockedTowers;
    std::list<Projectile *> lockedProjectiles;

    Enemy(std::string img, float x, float y, float radius, float speed, float hp, float damage, int money);

    virtual void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    virtual void Update(float deltaTime) override;
    virtual void Hit(float damage);
    virtual void Draw() const override;
    virtual bool IsTargetable() const { return true; }

    float GetRadius() const { return CollisionRadius; }
    float GetDamage() const { return damage; }
};
#endif // ENEMY_HPP
