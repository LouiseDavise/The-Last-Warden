#include <allegro5/color.h>

#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "StructureButton.hpp"

PlayScene *StructureButton::getPlayScene()
{
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
StructureButton::StructureButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Tower, float x, float y, int money) : ImageButton(img, imgIn, x, y), money(money), Base(Base), Tower(Tower)
{
}
void StructureButton::Update(float deltaTime)
{
    ImageButton::Update(deltaTime);
    if (getPlayScene()->GetMoney() >= money)
    {
        Enabled = true;
        Base.Tint = Tower.Tint = al_map_rgba(255, 255, 255, 255);
    }
    else
    {
        Enabled = false;
        Base.Tint = Tower.Tint = al_map_rgba(0, 0, 0, 160);
    }
}
void StructureButton::Draw() const
{
    ImageButton::Draw();
    Base.Draw();
    Tower.Draw();
}
