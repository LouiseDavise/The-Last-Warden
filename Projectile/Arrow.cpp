#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Arrow.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Tower;

Arrow::Arrow(Engine::Point position, Engine::Point forwardDirection, float rotation, Tower *parent) : Projectile("Projectiles/Arrow.png", 700, 1, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void Arrow::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("Effects/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
