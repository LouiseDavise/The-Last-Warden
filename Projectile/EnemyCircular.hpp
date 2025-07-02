#include "Projectile.hpp"

class EnemyCircular : public Projectile {
public:
    EnemyCircular(float x, float y, float vx, float vy, float damage = 10, float radius = 4.0f);
    void Update(float deltaTime) override;
    void OnExplode(Enemy* enemy) override;
};
