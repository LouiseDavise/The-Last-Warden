#pragma once
#include "Player.hpp"
#include "Weapon/SpearWeapon.hpp"

class SupportPlayer : public Player
{
public:
    explicit SupportPlayer(float x, float y);
    void Update(float dt) override;
    Engine::Point PositionWeapon() const override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> idleFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> walkFrames;

private:
    bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;
};
