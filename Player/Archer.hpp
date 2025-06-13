#ifndef ARCHER_HPP
#define ARCHER_HPP

#include "Player.hpp"

class BowWeapon;  // forward declaration

class Archer : public Player {
public:
    explicit Archer(float x, float y);
    void Update(float dt) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    Engine::Point PositionWeapon() const override;

    BowWeapon* GetBow() const { return bow; }

private:
    BowWeapon* bow;
};

#endif // ARCHER_HPP
