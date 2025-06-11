#ifndef TOWER_HPP
#define TOWER_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Structure/Structure.hpp"

class Enemy;
class PlayScene;

class Tower : public Structure {
protected:
    float coolDown;
    float range;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    Sprite imgBase;
    std::list<Tower *>::iterator lockedTowerIterator;
    PlayScene *getPlayScene();
    virtual void CreateProjectile() = 0;

public:
    Enemy *Target = nullptr;
    Tower(std::string imgBase, std::string imgTower, float x, float y, int price, float hp, float MAXhp, float radius, float coolDown, float range);
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // TOWER_HPP
