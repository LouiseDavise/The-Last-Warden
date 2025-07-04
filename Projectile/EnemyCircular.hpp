#include "Projectile.hpp"

class EnemyCircular : public Projectile {
public:
    EnemyCircular(float x, float y, float vx, float vy, float damage = 10, float radius = 4.0f);
    void Update(float deltaTime) override;
    void OnExplode(Enemy* enemy) override;

    float traveledDistance = 0;
    float maxTravelDistance = 1000;  // Or any value you want (e.g., 1000 pixels)
};
