#pragma once
#include "Weapon.hpp"

class Enemy;

class WandWeapon final : public Weapon
{
public:
    explicit WandWeapon(const Engine::Point &startPos, Player *owner = nullptr);

    void Use(float targetX, float targetY) override;
    void Update(float dt) override;
    bool IsFlying() const { return isFlying; }
    int GetQuota() const { return currentQuota; }
    int GetMaxQuota() const { return maxQuota; }
    bool IsCoolingDown() const { return coolingDown; }
    void Draw() const override;
    float GetCooldownPercent() const
    {
        return coolingDown ? cooldownTimer / cooldownTime : 1.0f;
    }
    void DrawAimLine() const;

private:
    // flight-state
    bool isFlying = false;
    Engine::Point velocity;
    float flightDist = 0.f;
    float maxDistance = 600.f;

    // helpers
    void TryHitEnemies();
    void TryReclaim();
    bool PointInsideRotatedRect(const Engine::Point &p,
                                const Engine::Point &centre,
                                float halfLen, float halfWidth,
                                float rotRad) const;

    int maxQuota = 10;
    int currentQuota = 10;
    float cooldownTime = 8.0f;
    float cooldownTimer = 0.0f;
    bool coolingDown = false;
};
