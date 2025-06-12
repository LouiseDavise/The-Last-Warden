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

private:
    bool keyUp = false,
         keyDown = false, keyLeft = false, keyRight = false;
    int damage = 1;
};
