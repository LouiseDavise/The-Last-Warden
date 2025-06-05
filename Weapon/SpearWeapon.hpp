#pragma once
#include "Weapon.hpp"

class Enemy;

/**
 *  A throwable spear:
 *   • starts in hand (available == true)
 *   • on Use() it flies until maxDistance OR manual reclaim
 *   • thin rotated-rectangle hit-box for damage
 */
class SpearWeapon final : public Weapon {
public:
    explicit SpearWeapon(const Engine::Point& startPos, Player* owner = nullptr);

    void Use   (float targetX, float targetY) override;
    void Update(float dt)              override;
    bool IsFlying() const { return isFlying; }

private:
    // flight-state
    bool            isFlying    = false;
    Engine::Point   velocity;
    float           flightDist  = 0.f;
    float           maxDistance = 500.f;   // pixels

    // helpers
    void TryHitEnemies();
    void TryReclaim();
    bool PointInsideRotatedRect(const Engine::Point& p,
                                const Engine::Point& centre,
                                float halfLen, float halfWidth,
                                float rotRad) const;
};
