#ifndef WALL_HPP
#define WALL_HPP

#include "Structure/Structure.hpp"

class PlayScene;

class Wall : public Structure {
protected:
    PlayScene *getPlayScene();
public:
    Wall(std::string img, float x, float y, float hp, float radius);
    float hp, MAXhp;
    void Hit(float damage);
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif // WALL_HPP