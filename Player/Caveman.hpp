#pragma once
#include "Player.hpp"
#include <memory>

class SpearWeapon;

class Caveman : public Player {
public:
    Caveman(float x, float y);

    void Update(float dt) override;
    void Draw()    const override;

    virtual void OnMouseDown(int button, int mx, int my) override;
    Engine::Point PositionWeapon() const override;

private:
    std::unique_ptr<SpearWeapon> spear;
};
