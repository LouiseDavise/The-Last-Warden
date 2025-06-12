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
    Dying,
    Attacking,
    Hurt
};

class Enemy : public Engine::Sprite
{
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float MAXhp;
    float damage;
    float atkcd;
    int money;
    State state;

    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> runFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> deathFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> attackFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> hurtFrames;
    float runTimer;
    float runInterval;
    float deathTimer;
    float deathInterval;
    float attackTimer;
    float attackInterval;
    float hurtTimer;
    float hurtInterval;
    int currentFrame;
    bool faceRight;

    PlayScene *getPlayScene();

public:
    std::list<Tower *> lockedTowers;
    std::list<Projectile *> lockedProjectiles;

    Enemy(std::string img, float x, float y, float radius, float speed, float hp, float MAXhp, float damage, float atkcd, int money);

    virtual void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    virtual void Update(float deltaTime) override;
    virtual void Hit(float damage);
    virtual void Draw() const override;

    float GetRadius() const { return CollisionRadius; }
    float GetDamage() const { return damage; }
    float GetHP() const { return hp; }
    State GetState() const {return state;}
};
#endif // ENEMY_HPP
