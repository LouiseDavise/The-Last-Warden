#pragma once
#include "Player.hpp"
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Weapon/SpearWeapon.hpp"
#include <memory>

class SpearWeapon;

class Spearman : public Player
{
public:
    Spearman(float x, float y);
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> spearLoopAudio;
    bool wasReturning = false;

    void Update(float dt) override;
    void Draw() const override;

    virtual void OnMouseDown(int button, int mx, int my) override;
    Engine::Point PositionWeapon() const override;
    SpearWeapon *GetSpear() const { return spear; }

private:
    SpearWeapon *spear = nullptr;
};
