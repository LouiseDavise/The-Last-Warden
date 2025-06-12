#ifndef MAGE_HPP
#define MAGE_HPP
#include "Player.hpp"

class WandWeapon;

class Mage : public Player {
public:
    explicit Mage(float x, float y);
    void Update(float dt) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    Engine::Point PositionWeapon() const override;
    WandWeapon* GetWand() const { return wand; }

private:
    WandWeapon* wand;
};

#endif // MAGE_HPP
