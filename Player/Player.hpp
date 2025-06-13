#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"
#include <vector>
#include <memory>
#include <string>

namespace Engine
{
    struct Point;
}

enum class PlayerState
{
    Idle,
    Walk,
    Hurt,
    Death
};

class Player : public Engine::Sprite
{
public:
    Player(std::string img, float x, float y, float hp, float MAXhp, float moveSpeed);

    void Hit(float dmg, const Engine::Point &from);
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);

    virtual void Update(float dt) override;
    virtual void Draw() const override;
    virtual void OnMouseDown(int button, int mx, int my);
    float GetMaxHP() const { return MAXhp; }

    bool FacingRight() const { return faceRight; }
    virtual Engine::Point PositionWeapon() const { return Position; }

    float GetHP() const { return hp; }
    float GetSpeed() const { return moveSpeed; }
    float GetRadius() const { return CollisionRadius; }
    PlayerState GetState() const { return state; }
    void HealPlayer(float value)
    {
        hp += value;
        if (hp > 100.0f)
            hp = 100.0f;
    }
    bool isDead = false;

protected:
    void UpdateMovement(float dt);
    void UpdateAnimation(float dt);
    void LoadAnimation(const std::string &prefix, int frames);

    bool keyW{}, keyA{}, keyS{}, keyD{};
    bool movingFlag = false;

    float hp;
    float MAXhp;
    float moveSpeed;
    float radius;
    Engine::Point knockbackVelocity = Engine::Point{0, 0};
    float knockbackTime = 0.0f; // duration left

    //---------------- Animation ----------------
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> idleFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> walkFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> hurtFrames;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> deathFrames;
    float walkTimer;
    float walkInterval;
    float deathTimer;
    float deathInterval;
    float idleTimer;
    float idleInterval;
    float hurtTimer;
    float hurtInterval;
    int currentFrame;
    bool faceRight = true;
    PlayerState state;
};

#endif
