#include "UI/Animation/ZukoKnockbackWave.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"

ZukoKnockbackWave::ZukoKnockbackWave(float x, float y)
    : Sprite("Effects/shockwave.png", x, y)
{
    waveBmp = Engine::Resources::GetInstance().GetBitmap("Effects/shockwave.png");
    Anchor = Engine::Point(0.5, 0.5);
}

void ZukoKnockbackWave::Update(float dt)
{
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    timer += dt;
    float progress = timer / duration;
    if (progress >= 1.0f)
    {
        scene->EffectGroup->RemoveObject(objectIterator);
        return;
    }

    float currentRadius = maxRadius * progress;

    // Knockback enemies
    for (auto &obj : scene->EnemyGroup->GetObjects())
    {
        Enemy *e = dynamic_cast<Enemy *>(obj);
        if (!e || !e->Visible)
            continue;

        if (Engine::Collider::IsCircleOverlap(Position, currentRadius, e->Position, e->CollisionRadius))
        {
            Engine::Point dir = e->Position - Position;
            if (dir.Magnitude() > 1e-3f)
                e->Position = e->Position + dir.Normalize() * 50 * (1 - progress);

            e->Hit(50);
        }
    }
    Sprite::Update(dt);
}

void ZukoKnockbackWave::Draw() const
{
    float alpha = (1.0f - timer / duration) * 255;
    float scale = 0.5 + 0.5 * (timer / duration); // expand slightly
    al_draw_tinted_scaled_rotated_bitmap(
        waveBmp.get(), al_map_rgba(100, 200, 255, static_cast<int>(alpha)),
        Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
        Position.x, Position.y,
        scale, scale, 0, 0);
}
