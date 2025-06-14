#include "SpearWeapon.hpp"
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "UI/Animation/ClashEffect.hpp"
#include "UI/Animation/AreaEffect.hpp"

using Engine::Point;
static constexpr float PI = 3.1415926f;

SpearWeapon::SpearWeapon(const Point &startPos, Player *owner)
    : Weapon("Weapons/Spear.png", 40, 750.f, startPos, owner)
{
    CollisionRadius = 0;
    Anchor = Point(0.5f, 0.5f);
    Size.x = 80;
    Size.y = 80;
    isLocked = false;
    spinWaitDuration = 1.0f;
    spinSpeed = 10.0f;
}

void SpearWeapon::Use(float tx, float ty)
{
    if (isLocked)
        return;

    // Manual return click while stuck
    if (isStuck && !isSpinningBeforeReturn && !isReturning)
    {
        isStuck = false;
        isSpinningBeforeReturn = true;
        isLocked = true;
        spinWaitTimer = 0.0f;
        return;
    }

    if (!available || isFlying || coolingDown || currentQuota <= 0)
        return;

    Point dir = Point(tx, ty) - Position;
    if (dir.Magnitude() < 1e-3f)
        return;

    dir = dir.Normalize();
    velocity = dir * speed;
    Rotation = std::atan2(dir.y, dir.x);
    isFlying = true;
    available = false;
    flightDist = 0.f;
    hitEnemiesForward.clear();
    hitEnemiesReturn.clear();
    hitEnemies.clear();

    auto *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!(scene && scene->IsMochiHealing()))
    {
        currentQuota--;
        if (currentQuota == 0)
        {
            coolingDown = true;
            cooldownTimer = 0.0f;
        }
    }
}

void SpearWeapon::Update(float dt)
{
    Sprite::Update(dt);

    if (coolingDown)
    {
        cooldownTimer += dt;
        if (cooldownTimer >= cooldownTime)
        {
            cooldownTimer = 0;
            currentQuota = maxQuota;
            coolingDown = false;
        }
    }

    if (isFlying)
    {
        Point disp = velocity * dt;
        Position = Position + disp;
        flightDist += disp.Magnitude();
        TryHitEnemies();

        float mapW = PlayScene::MapWidth * PlayScene::BlockSize;
        float mapH = (PlayScene::MapHeight + 3) * PlayScene::BlockSize;

        if (flightDist >= maxDistance || Position.x < 0 || Position.x > mapW || Position.y < 0 || Position.y > mapH)
        {
            isFlying = false;
            isStuck = true;
            velocity = Point(0, 0);
            return;
        }
    }
    else
    {
        if (isSpinningBeforeReturn)
        {
            spinWaitTimer += dt;
            Rotation += spinSpeed * dt;
            if (Rotation > 2 * PI)
                Rotation -= 2 * PI;

            if (spinWaitTimer >= spinWaitDuration)
            {
                isSpinningBeforeReturn = false;
                isReturning = true;
                hitEnemies.clear();
                if (owner)
                {
                    Point dir = owner->Position - Position;
                    velocity = dir.Normalize() * speed * 0.8f;
                }
            }
        }
        else if (isReturning)
        {
            if (owner)
            {
                Point dir = owner->Position - Position;
                float dist = dir.Magnitude();

                if (dist > 1e-3)
                {
                    dir = dir.Normalize();
                    velocity = dir * speed * 0.8f;
                }
            }

            Position = Position + velocity * dt;

            Rotation += spinSpeed * dt;
            if (Rotation > 2 * PI)
                Rotation -= 2 * PI;
            TryHitEnemies();
        }

        TryReclaim();

        if (!isSpinningBeforeReturn && !isReturning)
            Weapon::Update(dt);
    }
}

void SpearWeapon::TryHitEnemies()
{
    PlayScene *scene = GetPlayScene();
    if (!scene)
        return;

    float halfLen = Size.x * 0.5f;
    float halfW = std::max(4.0f, Size.y * 0.25f);

    for (auto *obj : scene->EnemyGroup->GetObjects())
    {
        auto *e = dynamic_cast<Enemy *>(obj);
        if (!e || !e->Visible)
            continue;

        bool alreadyHit =
            (isReturning && hitEnemiesReturn.count(e)) ||
            (!isReturning && hitEnemiesForward.count(e));

        if (alreadyHit)
            continue;

        if (PointInsideRotatedRect(e->Position, Position, halfLen, halfW + e->GetRadius(), Rotation))
        {
            int dealtDamage = isReturning ? (damage / 2) : damage;
            e->Hit(dealtDamage);

            if (isReturning)
                hitEnemiesReturn.insert(e);
            else
                hitEnemiesForward.insert(e);

            scene->EffectGroup->AddNewObject(new ClashEffect(e->Position.x, e->Position.y));
            scene->EffectGroup->AddNewObject(new AreaEffect(e->Position.x, e->Position.y, isReturning ? 50.0f : 64.0f, 0.5f, al_map_rgb(255, 244, 79)));
        }
    }
}

void SpearWeapon::TryReclaim()
{
    auto *scene = GetPlayScene();
    if (!scene || !owner)
        return;

    const float pickR = 35.f;
    if ((owner->Position - Position).MagnitudeSquared() <= pickR * pickR)
    {
        isFlying = false;
        isReturning = false;
        isSpinningBeforeReturn = false;
        isStuck = false;
        isLocked = false;
        velocity = Point(0, 0);
        Reclaim();
    }
}

bool SpearWeapon::PointInsideRotatedRect(const Point &p, const Point &c, float halfL, float halfW, float rotRad) const
{
    float cosR = std::cos(-rotRad);
    float sinR = std::sin(-rotRad);
    float dx = p.x - c.x;
    float dy = p.y - c.y;
    float lx = dx * cosR - dy * sinR;
    float ly = dx * sinR + dy * cosR;
    return std::abs(lx) <= halfL && std::abs(ly) <= halfW;
}

void SpearWeapon::Draw() const
{
    if (!isFlying && !isReturning && !isSpinningBeforeReturn && available && owner)
    {
        auto scene = GetPlayScene();
        if (scene)
        {
            Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();
            Engine::Point worldMouse = mouse + scene->camera;

            Point dir = worldMouse - Position;
            if (dir.Magnitude() >= 1e-3f)
            {
                Point end = Position + dir.Normalize() * maxDistance;
                al_draw_line(Position.x, Position.y, end.x, end.y, al_map_rgb(237, 255, 181), 0.1f);
            }
        }
    }

    Weapon::Draw();

    if (PlayScene::DebugMode && isFlying)
    {
        float halfLen = Size.x * 0.5f;
        float halfW = std::max(4.0f, Size.y * 0.25f);
        float cosR = std::cos(Rotation);
        float sinR = std::sin(Rotation);

        Point corners[4];
        corners[0] = Position + Point(-halfLen * cosR - halfW * sinR, -halfLen * sinR + halfW * cosR);
        corners[1] = Position + Point(halfLen * cosR - halfW * sinR, halfLen * sinR + halfW * cosR);
        corners[2] = Position + Point(halfLen * cosR + halfW * sinR, halfLen * sinR - halfW * cosR);
        corners[3] = Position + Point(-halfLen * cosR + halfW * sinR, -halfLen * sinR - halfW * cosR);

        for (int i = 0; i < 4; ++i)
            al_draw_line(corners[i].x, corners[i].y, corners[(i + 1) % 4].x, corners[(i + 1) % 4].y, al_map_rgb(255, 0, 0), 2);
    }
}