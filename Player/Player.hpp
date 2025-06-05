#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"
#include <vector>
#include <memory>
#include <string>

namespace Engine { struct Point; }

class Player : public Engine::Sprite {
public:
    Player(float x, float y, float hp, float moveSpeed,
           const std::string& firstFramePath, int frames, float animFPS);

    void   TakeDamage(float dmg);
    void   OnKeyDown(int keyCode);
    void   OnKeyUp  (int keyCode);

    virtual void Update(float dt) override;
    virtual void Draw()   const  override;
    virtual void OnMouseDown(int button, int mx, int my);

    float  GetHP()    const { return hp; }
    float  GetSpeed() const { return moveSpeed; }
    bool  FacingRight() const { return faceRight; }
    virtual Engine::Point PositionWeapon() const {return Position;}

protected:
    void UpdateMovement(float dt);
    void UpdateAnimation(float dt);
    void LoadAnimation(const std::string& prefix, int frames);

    bool keyW{}, keyA{}, keyS{}, keyD{};
    bool faceRight = true;
    bool movingFlag = false;

private:
    float  hp;
    float  moveSpeed;

    //---------------- Animation ----------------
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> framesRight;
    std::shared_ptr<ALLEGRO_BITMAP> currentBitmap;
    int   currentFrame   = 0;
    float animInterval   = 0.1f;   // seconds / frame
    float animTimer      = 0.0f;   // accum.
};


#endif
