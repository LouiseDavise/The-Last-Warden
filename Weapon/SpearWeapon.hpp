#pragma once
#include "Weapon.hpp"
#include <unordered_set>

class Enemy;

/**
 *  A throwable spear:
 *   • starts in hand (available == true)
 *   • on Use() it flies until maxDistance OR manual reclaim
 *   • thin rotated-rectangle hit-box for damage
 */
class SpearWeapon final : public Weapon
{
public:
    explicit SpearWeapon(const Engine::Point &startPos, Player *owner = nullptr);

    void Use(float targetX, float targetY) override;
    void Update(float dt) override;
    bool IsFlying() const { return isFlying; }
    int GetQuota() const { return currentQuota; }
    int GetMaxQuota() const { return maxQuota; }
    bool IsCoolingDown() const { return coolingDown; }
    float GetCooldownPercent() const
    {
        return coolingDown ? cooldownTimer / cooldownTime : 1.0f;
    }
    void Draw() const override;

protected:
    std::unordered_set<Enemy *> hitEnemies;

private:
    // flight-state
    bool isFlying = false;
    Engine::Point velocity;
    float flightDist = 0.f;
    float maxDistance = 500.f; // pixels

    // helpers
    void TryHitEnemies();
    void TryReclaim();
    bool PointInsideRotatedRect(const Engine::Point &p,
                                const Engine::Point &centre,
                                float halfLen, float halfWidth,
                                float rotRad) const;

    int maxQuota = 4;
    int currentQuota = 4;
    float cooldownTime = 10.0f;
    float cooldownTimer = 0.0f;
    bool coolingDown = false;
};
