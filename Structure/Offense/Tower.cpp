#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>

#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/LOG.hpp"
#include "Tower.hpp"

PlayScene *Tower::getPlayScene()
{
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Tower::Tower(std::string imgBase, std::string imgTower, float x, float y, int price, float hp, float MAXhp, float radius, float coolDown, float range)
    : Structure(imgTower, x, y, price, hp, MAXhp, radius), imgBase(imgBase, x, y), coolDown(coolDown), range(range) {
        type = StructureType::Offense;
    }

void Tower::Update(float deltaTime)
{
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;
    if (!Enabled)
        return;
    if (Target)
    {
        Engine::Point diff = Target->Position - Position;
        if (diff.Magnitude() > range)
        {
            Target->lockedTowers.erase(lockedTowerIterator);
            Target = nullptr;
            lockedTowerIterator = std::list<Tower *>::iterator();
        }
    }
    if (!Target)
    {
        // Lock first seen target.
        // Can be improved by Spatial Hash, Quad Tree, ...
        // However simply loop through all enemies is enough for this program.
        for (auto &it : scene->EnemyGroup->GetObjects())
        {
            Enemy *enemy = dynamic_cast<Enemy *>(it);
            if (!enemy || enemy->GetState() == State::Dying)
                continue;
            Engine::Point diff = it->Position - Position;
            if (diff.Magnitude() <= range)
            {
                Target = dynamic_cast<Enemy *>(it);
                Target->lockedTowers.push_back(this);
                lockedTowerIterator = std::prev(Target->lockedTowers.end());
                break;
            }
        }
    }
    if (Target)
    {
        Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
        Engine::Point targetRotation = (Target->Position - Position).Normalize();
        float maxRotateRadian = rotateRadian * deltaTime;
        float cosTheta = originRotation.Dot(targetRotation);
        // Might have floating-point precision error.
        if (cosTheta > 1)
            cosTheta = 1;
        else if (cosTheta < -1)
            cosTheta = -1;
        float radian = acos(cosTheta);
        Engine::Point rotation;
        if (abs(radian) <= maxRotateRadian)
            rotation = targetRotation;
        else
            rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
        // Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
        Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
        // Shoot reload.
        reload -= deltaTime;
        if (reload <= 0)
        {
            // shoot.
            reload = coolDown;
            CreateProjectile();
        }
    }
}
void Tower::Draw() const
{
    if (Preview)
    {
        al_draw_filled_circle(Position.x, Position.y, range, al_map_rgba(0, 255, 0, 50));
    }
    if (hp > 0) {
        const float barW = 40;
        const float barH = 6;
        const float yOff = CollisionRadius + 10;
        const float left = Position.x - barW / 2;
        const float right = Position.x + barW / 2;
        const float top = Position.y - yOff;
        const float fillW = barW * (hp / MAXhp);

        al_draw_filled_rectangle(left, top, right, top + barH, al_map_rgb(40, 40, 40));

        float ratio = hp / MAXhp;
        ALLEGRO_COLOR color = al_map_rgb(100 + 50 * (1 - ratio), 150 + 50 * (1 - ratio), 255);  // Blue tone
        al_draw_filled_rectangle(left, top, left + fillW, top + barH, color);

        al_draw_rectangle(left, top, right, top + barH, al_map_rgb(255, 255, 255), 1);
    }
    imgBase.Draw();
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(50, 50, 255), 2);
    }
}

void Tower::Hit(float damage) {
    hp -= damage;
    if (hp <= 0) {
        Enabled = false;
        Target = nullptr;
        getPlayScene()->mapState[occupyY][occupyX] = getPlayScene()->getTileType(0);
        getPlayScene()->StructureGroup->RemoveObject(objectIterator);
    }
}