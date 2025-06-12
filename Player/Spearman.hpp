#pragma once
#include "Player.hpp"
#include "Weapon/SpearWeapon.hpp"
#include <memory>

class SpearWeapon;

class Spearman : public Player
{
public:
    Spearman(float x, float y);

    void Update(float dt) override;
    void Draw() const override;

    virtual void OnMouseDown(int button, int mx, int my) override;
    Engine::Point PositionWeapon() const override;
    SpearWeapon *GetSpear() const { return spear; }

private:
    SpearWeapon *spear = nullptr;
};
