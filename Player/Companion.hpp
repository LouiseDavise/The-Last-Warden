#pragma once
#include "Player.hpp"
#include "Weapon/SpearWeapon.hpp"

class Companion : public Player
{
public:
    explicit Companion(float x, float y);
    void Update(float dt) override;
    Engine::Point PositionWeapon() const override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> idleFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> walkFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> attackFrames;
    std::shared_ptr<ALLEGRO_BITMAP> currentBitmap;
    bool isAttacking = false;
    float attackTimer = 0;
    const float attackDuration = 0.4f;
    float GetRadius() const;
    void TeleportToPlayer();
    float attackQuota = 100.0f;
    float quotaDepleteRate = 50.0f;
    float quotaRechargeTime = 2.0f;
    float timeSinceLastAttack = 0.0f;
    const float rechargeDelay = 4.0f;
    bool isRecharging = false;
    void Draw() const override;
    std::string companionName;
    float wispSkillCooldown = 10.0f;
    float wispSkillTimer = 0.0f;
    bool wispSkillReady = true;
    bool IsWispSkillReady() const { return wispSkillReady; }
    float GetWispCooldownProgress() const
    {
        return wispSkillReady ? 1.0f : wispSkillTimer / wispSkillCooldown;
    }

    bool IsMochiSkillReady() const { return mochiSkillReady; }
    float GetMochiCooldownProgress() const
    {
        return mochiSkillReady ? 1.0f : mochiSkillTimer / mochiSkillCooldown;
    }
    float mochiSkillCooldown = 32.0f;
    float mochiSkillTimer = 0.0f;
    bool mochiSkillReady = true;
    bool mochiSkillActive = false;
    float mochiHealRadius = 200.0f;
    float mochiHealAmount = 35.0f;
    float mochiSkillDisplayTime = 1.5f;
    float mochiSkillVisualTimer = 0.0f;
    bool mochiHealingOngoing = false;
    float mochiHealRate = 15.0f;
    float mochiHealDuration = 5.0f;
    float mochiHealElapsed = 0.0f;
    bool IsMochiHealingOngoing() const { return mochiHealingOngoing; }

    float zukoSkillCooldown = 23.0f;
    float zukoSkillTimer = 0.0f;
    bool zukoSkillReady = true;
    bool IsZukoSkillReady() const { return zukoSkillReady; }
    float GetzukoSkillCooldownProgress() const
    {
        return zukoSkillReady ? 1.0f : zukoSkillTimer / zukoSkillCooldown;
    }
    void Reset();

private:
    bool keyUp = false,
         keyDown = false, keyLeft = false, keyRight = false;
    int damage = 1;
    float animTimer = 0.0f;
    float animInterval = 0.1f;
};
