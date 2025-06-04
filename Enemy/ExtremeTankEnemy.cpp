#include <iostream>
#include "ExtremeTankEnemy.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Structure/Turret/Turret.hpp"
#include "Bullet/Bullet.hpp"

ExtremeTankEnemy::ExtremeTankEnemy(int x, int y)
    : Enemy("play/enemy-5.png", x, y, 18, 100, 55, 35),
      armorOverlay("play/armor-overlay-extreme.png", x, y)
{
    armorOverlay.Anchor = Anchor;
    shieldToggleTimer = shieldToggleInterval;
}

void ExtremeTankEnemy::Hit(float damage)
{
    if (hp <= 0)
        return;

    float actualDamage = shieldActive ? damage * (0) : damage;
    hp -= actualDamage;

    if (hp <= 0)
    {
        OnExplode();
        auto *scene = getPlayScene();

        for (auto &it : lockedTurrets)
            it->Target = nullptr;
        for (auto &it : lockedBullets)
            it->Target = nullptr;

        scene->EarnMoney(money);
        scene->EnemyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}

void ExtremeTankEnemy::Update(float deltaTime)
{
    shieldToggleTimer -= deltaTime;
    if (shieldToggleTimer <= 0)
    {
        shieldToggleTimer = shieldToggleInterval;
        shieldActive = (rand() % 2 == 0);
    }

    armorOverlay.Position = Position;
    Enemy::Update(deltaTime);
}

void ExtremeTankEnemy::Draw() const
{
    Enemy::Draw();
    if (shieldActive)
    {
        armorOverlay.Draw();
    }
}

bool ExtremeTankEnemy::IsTargetable() const
{
    return !shieldActive;
}
