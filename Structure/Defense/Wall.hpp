#ifndef WALL_HPP
#define WALL_HPP

#include "Structure/Structure.hpp"

class Wall : public Structure {
public:
    Wall(std::string img, float x, float y, float hp, float radius);
    float hp, MAXhp;
    void Hit(float damage);
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif // WALL_HPP