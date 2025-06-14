#pragma once
#include "Weapon.hpp"
#include <unordered_set>

class Enemy;

/**
 *  A throwable spear:
 *   • starts in hand (available == true)
 *   • on Use() it flies until maxDistance OR collides with map bounds
 *   • when stuck: click again to rotate and return
 *   • thin rotated-rectangle hit-box for damage
 */
class SpearWeapon final : public Weapon
{
public:
    explicit SpearWeapon(const Engine::Point &startPos, Player *owner = nullptr);

    void Use(float targetX, float targetY) override;
    void Update(float dt) override;
    void Draw() const override;

    bool IsFlying() const { return isFlying; }
    int GetQuota() const { return currentQuota; }
    int GetMaxQuota() const { return maxQuota; }
    bool IsCoolingDown() const { return coolingDown; }
    float GetCooldownPercent() const
    {
        return coolingDown ? cooldownTimer / cooldownTime : 1.0f;
    }
    bool IsSpinningBeforeReturn() const { return isSpinningBeforeReturn; }
    bool IsReturning() const { return isReturning; }
    std::unordered_set<Enemy *> hitEnemiesForward;
    std::unordered_set<Enemy *> hitEnemiesReturn;

protected:
    std::unordered_set<Enemy *> hitEnemies;

private:
    // --- Spear State ---
    bool isFlying = false;
    bool isStuck = false;
    bool isReturning = false;
    bool isSpinningBeforeReturn = false;
    bool isLocked = false;

    Engine::Point velocity;
    float flightDist = 0.0f;
    float maxDistance = 500.0f;

    // --- Spin Before Return ---
    float spinWaitTimer = 0.0f;
    float spinWaitDuration = 1.0f; // time before return starts
    float spinSpeed = 10.0f;       // radians per second

    // --- Quota & Cooldown ---
    int maxQuota = 4;
    int currentQuota = 4;
    float cooldownTime = 10.0f;
    float cooldownTimer = 0.0f;
    bool coolingDown = false;

    // --- Helpers ---
    void TryHitEnemies();
    void TryReclaim();
    bool PointInsideRotatedRect(const Engine::Point &p,
                                const Engine::Point &center,
                                float halfLen, float halfWidth,
                                float rotRad) const;
};
