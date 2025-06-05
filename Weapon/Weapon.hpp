#pragma once
#include <string>
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"

class Player;
class PlayScene;

class Weapon : public Engine::Sprite {
public:
    Weapon(const std::string& bmpPath,
           float   dmg,
           float   projectileSpeed,
           const   Engine::Point& startPos,
           Player* owner = nullptr);

    virtual ~Weapon() = default;

    virtual void Use(float targetX, float targetY)         = 0;  
    virtual void Update(float dt)   override;              // default = just Sprite
    virtual void Draw()   const     override;              // default Sprite draw
    virtual void Reclaim();                                // return to owner’s hand

    bool        IsAvailable() const { return available; }
    void        SetOwner(Player* p) { owner = p; }

protected:
    PlayScene*  GetPlayScene() const;

    float  damage   = 0.f;
    float  speed    = 0.f;
    bool   available = true;        
    Player* owner    = nullptr;
};
