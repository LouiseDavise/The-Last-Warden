#pragma once
#include "Weapon.hpp"

class BowWeapon final : public Weapon
{
public:
    explicit BowWeapon(const Engine::Point &startPos, Player *owner = nullptr);
    void Use(float targetX, float targetY) override;
    void Update(float dt) override;

    int GetQuota() const { return currentQuota; }
    int GetMaxQuota() const { return maxQuota; }
    bool IsCoolingDown() const { return coolingDown; }
    float GetCooldownPercent() const
    {
        return coolingDown ? cooldownTimer / cooldownTime : 1.0f;
    }
    void Draw() const override;

private:
    int maxQuota = 8;
    int currentQuota = 6;
    float cooldownTime = 3.0f;
    float cooldownTimer = 0.0f;
    bool coolingDown = false;
    float maxDistance = 650.0f;
};
