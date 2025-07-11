#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "HighOrc.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Collider.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Structure/Structure.hpp"
#include <cmath>

HighOrc::HighOrc(float x, float y)
    : Enemy("Enemies/HighOrc/Run/image1x1.png", x, y, 48, 60, 800, 800, 25, 0.75f, 20)
{
    Size.x = 192;
    Size.y = 192;
    idleMark = 6;
    for (int i = 1; i <= 6; ++i)
    {
        std::string path = "Enemies/HighOrc/Run/image" + std::to_string(i) + "x1.png";
        runFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/HighOrc/Charge/image" + std::to_string(i) + "x1.png";
        chargeFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 8; ++i)
    {
        std::string path = "Enemies/HighOrc/Death/image" + std::to_string(i) + "x1.png";
        deathFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 10; ++i)
    {
        std::string path;
        if(i <= idleMark) path = "Enemies/HighOrc/Attack/image" + std::to_string(i) + "x1.png";
        else path = "Enemies/HighOrc/Idle/image" + std::to_string(i-idleMark) + "x1.png";
        attackFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for (int i = 1; i <= 6; ++i)
    {
        std::string path = "Enemies/HighOrc/Hurt/image" + std::to_string(i) + "x1.png";
        hurtFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }

    for(int i = 1; i <= 4; i++)
    {
        std::string path = "Enemies/HighOrc/Idle/image" + std::to_string(i) + "x1.png";
        blinkFrames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }


    // Initial image
    bmp = runFrames[0];
    Velocity = Engine::Point(0, 0);
    state = State::Run;
    runTimer = 0;
    runInterval = 0.12f;
    chargeTimer = 0;
    chargeInterval = 0.09f;
    blinkTimer = 0;
    blinkInterval = 0.12f;
    deathTimer = 0;
    deathInterval = 0.12f;
    attackTimer = 0;
    attackInterval = 0.055f;
    hurtTimer = 0;
    hurtInterval = 0.05f;
    currentFrame = 0;
}

void HighOrc::Update(float deltaTime)
{
    auto* scene = getPlayScene();
    auto* player = scene->GetPlayer();
    bool playerCollide = player && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, player->Position, player->CollisionRadius) && player->GetHP() > 0 && hp > 0;
    Structure* collidedStructure = nullptr;
    for (auto* obj : scene->StructureGroup->GetObjects()) {
        Structure* structure = dynamic_cast<Structure*>(obj);
        if (structure && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, structure->Position, structure->CollisionRadius)) {
            collidedStructure = structure;
            break;
        }
    }

    // Run state behavior and transition into Charging
    if (state == State::Blinking){
        blinkTimer += deltaTime;
        if(blinkTimer >= blinkInterval){
            blinkTimer = 0;
            currentFrame = (currentFrame + 1) % blinkFrames.size();
            bmp = blinkFrames[currentFrame];
        }

        blinkCooldownTimer += deltaTime;
        if(blinkCooldownTimer >= blinkCooldown){
            state = State::Charging;
            currentFrame = 0;
            chargeTimer = 0;
            speed = chargeSpeed;
            chargeCooldownTimer = 0;
            blinkCooldownTimer = 0;
            Velocity = (chargeTargetPos - Position).Normalize() * speed;
            return;
        }
        return;
    }

    if(state == State::Run || state == State::Hurt) chargeCooldownTimer += deltaTime;

    if (state == State::Run) {
        speed = normalSpeed;
        runTimer += deltaTime;
        if (runTimer >= runInterval) {
            runTimer = 0;
            currentFrame = (currentFrame + 1) % runFrames.size();
            bmp = runFrames[currentFrame];
        }

        bool shouldCharge = false;
        if (chargeCooldownTimer >= chargeCooldown) {
            if (player && player->GetHP() > 0 &&
                Engine::Collider::IsCircleOverlap(Position, chargeRange, player->Position, player->CollisionRadius)) {
                shouldCharge = true;
                chargeTargetPos = player->Position;
            } else {
                for (auto& it : scene->StructureGroup->GetObjects()) {
                    Structure* s = dynamic_cast<Structure*>(it);
                    if (!s || s->GetHP() <= 0) continue;
                    if (Engine::Collider::IsCircleOverlap(Position, chargeRange, s->Position, s->CollisionRadius)) {
                        shouldCharge = true;
                        chargeTargetPos = s->Position;
                        break;
                    }
                }
            }
        }

        if(shouldCharge){
            faceRight = chargeTargetPos.x > Position.x;
            state = State::Blinking;
            chargeOrigin = Position;
            Engine::Point dir = (chargeTargetPos - Position).Normalize();
            Engine::Point testPos = Position;
            float maxDist = stopRange;
            float step = 8.0f;
            float traveled = 0;

            while (traveled < maxDist) {
                Engine::Point next = testPos + dir * step;
                int tileX = static_cast<int>(next.x) / PlayScene::BlockSize;
                int tileY = static_cast<int>(next.y) / PlayScene::BlockSize;

                if (!scene->IsWalkable(tileX, tileY))
                    break;

                testPos = next;
                traveled += step;
            }
            chargeDestination = testPos;
            currentFrame = 0;
            Velocity = Engine::Point(0, 0);
            return;
        }else{
            faceRight = Velocity.x > 0;
        }
    }

    // Charging logic
    else if (state == State::Charging) {
        chargeTimer += deltaTime;

        if (chargeTimer >= chargeInterval) {
            chargeTimer = 0;
            currentFrame = (currentFrame + 1) % chargeFrames.size();
            bmp = chargeFrames[currentFrame];
        }

        if (playerCollide) {
            state = State::Attacking;
            speed = normalSpeed;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0);
            player->Hit(GetDamage(), Position);
            return;
        } else if (collidedStructure) {
            state = State::Attacking;
            speed = normalSpeed;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0);
            collidedStructure->Hit(GetDamage());
            return;
        }

        // Movement during charge
        Position.x += Velocity.x * deltaTime;
        Position.y += Velocity.y * deltaTime;

        float dx = Position.x - chargeDestination.x;
        float dy = Position.y - chargeDestination.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance <= speed * deltaTime) { // close enough
            Position = chargeDestination; // snap to final spot to avoid overshooting
            state = State::Run;
            speed = normalSpeed;
            currentFrame = 0;
        }

        return;
    }

    else if (state == State::Dying)
    {
        if (deathFrames.empty())
        {
            getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
            return;
        }
        deathTimer += deltaTime;
        if (deathTimer >= deathInterval && deathFrames.size() > 0)
        {
            deathTimer = 0;
            currentFrame++;
            if (currentFrame >= deathFrames.size())
            {
                getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
                currentFrame = 0;
                return;
            }
        }
        bmp = deathFrames[currentFrame]; // Set death frame
        return;
    }

    else if (state == State::Attacking) {
        attackTimer += deltaTime;
        cooldownTimer += deltaTime;
        if(attackTimer >= attackInterval){
            attackTimer = 0;
            if(currentFrame < idleMark - 1)currentFrame++;
            if(currentFrame >= idleMark - 1){
                if(!playerCollide && !collidedStructure){
                    state = State::Run;
                    currentFrame = 0;
                }
                currentFrame++;
                if (currentFrame >= attackFrames.size())
                    currentFrame = idleMark;
            }

        }
        
        if (cooldownTimer >= atkcd) {
            cooldownTimer = 0;
            currentFrame = 0;

            if (playerCollide) {
                player->Hit(GetDamage(), Position);
            } else if (collidedStructure) {
                collidedStructure->Hit(GetDamage());
            }else{
                state = State::Run;
            }
        }

        return;
    }

    else if (state == State::Hurt)
    {
        hurtTimer += deltaTime;
        if (hurtTimer >= hurtInterval)
        {
            hurtTimer = 0;
            currentFrame++;
            if (currentFrame >= hurtFrames.size())
            {
                state = State::Run;
                currentFrame = 0;
                return;
            }
        }
        bmp = hurtFrames[currentFrame];
        return;
    }

    if (playerCollide)
    {
        if (state != State::Attacking)
        {
            state = State::Attacking;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0); // Stop movement
        }
        player->Hit(GetDamage(), Position);
    }
    else if (collidedStructure)
    {
        if (state != State::Attacking)
        {
            state = State::Attacking;
            currentFrame = 0;
            attackTimer = 0;
            Velocity = Engine::Point(0, 0); // Stop movement
        }
        collidedStructure->Hit(GetDamage());
    }
    else{
        state = State::Run;
        int gx = static_cast<int>(Position.x) / PlayScene::BlockSize;
        int gy = static_cast<int>(Position.y) / PlayScene::BlockSize;

        if (gx < 0 || gx >= PlayScene::MapWidth || gy < 0 || gy >= PlayScene::MapHeight)
            return;

        int currDist = scene->mapDistance[gy][gx];
        if (currDist <= 0)
            return;

        Engine::Point avgDir(0, 0);
        int count = 0;

        for (const auto &dir : PlayScene::directions)
        {
            int nx = gx + dir.x;
            int ny = gy + dir.y;
            if (nx < 0 || nx >= PlayScene::MapWidth || ny < 0 || ny >= PlayScene::MapHeight)
                continue;
            if (!scene->IsWalkable(nx, ny))
                continue;

            int neighborDist = scene->mapDistance[ny][nx];
            if (neighborDist >= 0 && neighborDist < currDist)
            {
                avgDir.x += dir.x;
                avgDir.y += dir.y;
                count++;
            }
        }

        if (count > 0)
        {
            avgDir.x /= count;
            avgDir.y /= count;

            float len = std::sqrt(avgDir.x * avgDir.x + avgDir.y * avgDir.y);
            if (len > 0)
            {
                Engine::Point norm = avgDir / len;
                Velocity = norm * speed;
            }
            else
            {
                Velocity = Engine::Point(0, 0);
            }
        }
    }
    // Movement with slide-along-wall fallback
    float nextX = Position.x + Velocity.x * deltaTime;
    float nextY = Position.y + Velocity.y * deltaTime;

    int tileX = static_cast<int>(nextX) / PlayScene::BlockSize;
    int tileY = static_cast<int>(nextY) / PlayScene::BlockSize;

    bool moved = false;

    if (scene->IsWalkable(tileX, tileY))
    {
        Position.x = nextX;
        Position.y = nextY;
        moved = true;
    }
    else
    {
        // Try moving only X
        float testX = Position.x + Velocity.x * deltaTime;
        int testTileX = static_cast<int>(testX) / PlayScene::BlockSize;
        if (scene->IsWalkable(testTileX, static_cast<int>(Position.y) / PlayScene::BlockSize))
        {
            Position.x = testX;
            moved = true;
        }

        // Try moving only Y
        float testY = Position.y + Velocity.y * deltaTime;
        int testTileY = static_cast<int>(testY) / PlayScene::BlockSize;
        if (scene->IsWalkable(static_cast<int>(Position.x) / PlayScene::BlockSize, testTileY))
        {
            Position.y = testY;
            moved = true;
        }
    }

    if (!moved)
        Velocity = Engine::Point(0, 0);
}

void HighOrc::Draw() const
{
    ALLEGRO_BITMAP *frame;
    switch (state)
    {
    case State::Run:
        frame = runFrames[currentFrame].get();
        break;
    case State::Dying:
        frame = deathFrames[currentFrame].get();
        break;
    case State::Attacking:
        frame = attackFrames[currentFrame].get();
        break;
    case State::Hurt:
        frame = hurtFrames[currentFrame].get();
        break;
    case State::Charging:
        frame = chargeFrames[currentFrame].get();
        break;
    case State::Blinking:
        frame = blinkFrames[currentFrame].get();
        break;
    }
    float cx = Anchor.x * al_get_bitmap_width(frame);
    float cy = Anchor.y * al_get_bitmap_height(frame);
    float scaleX = Size.x / al_get_bitmap_width(frame);
    float scaleY = Size.y / al_get_bitmap_height(frame);
    int flags = faceRight ? ALLEGRO_FLIP_HORIZONTAL : 0;
    ALLEGRO_COLOR finalTint = Tint;

    if (state == State::Blinking) {
        // Make blinking sprite dim with pulsing brightness (0.5–0.85 range)
        float pulse = 0.5f + 0.35f * std::sin(al_get_time() * 12);
        finalTint = al_map_rgba_f(pulse, pulse, pulse, 1.0f);    // RGB dimmed, alpha = 1

        ALLEGRO_COLOR pathColor = al_map_rgba(255, 120, 120, 80);

        float dx = chargeDestination.x - chargeOrigin.x;
        float dy = chargeDestination.y - chargeOrigin.y;
        float length = std::sqrt(dx * dx + dy * dy);
        float angle = std::atan2(dy, dx);
        float width = 100.0f;
        float hw = width * 0.5f;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        Engine::Point offsets[4] = {
            Engine::Point(0, -hw),
            Engine::Point(length, -hw),
            Engine::Point(length, hw),
            Engine::Point(0, hw),
        };

        ALLEGRO_VERTEX verts[4];
        for (int i = 0; i < 4; ++i) {
            float lx = offsets[i].x;
            float ly = offsets[i].y;
            verts[i].x = chargeOrigin.x + lx * cosA - ly * sinA;
            verts[i].y = chargeOrigin.y + lx * sinA + ly * cosA;
            verts[i].z = 0;
            verts[i].color = pathColor;
            verts[i].u = 0;
            verts[i].v = 0;
        }

        al_draw_prim(verts, nullptr, nullptr, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
    }

    al_draw_tinted_scaled_rotated_bitmap(frame, finalTint, cx, cy, Position.x, Position.y, scaleX, scaleY, Rotation, flags);

    if (hp > 0)
    {
        const float barW = 40;                   // width  of the bar (pixels)
        const float barH = 6;                    // height of the bar
        const float yOff = CollisionRadius + 12; // 12 px above sprite
        const float left = Position.x - barW / 2;
        const float right = Position.x + barW / 2;
        const float top = Position.y - yOff;
        const float fillW = barW * (hp / MAXhp);

        // background (dark grey)
        al_draw_filled_rectangle(left, top, right, top + barH,
                                 al_map_rgb(40, 40, 40));
        // foreground (green → red as HP drops)
        float ratio = hp / MAXhp;
        ALLEGRO_COLOR col = al_map_rgb(255 * (1 - ratio), 255 * ratio, 0);
        al_draw_filled_rectangle(left, top, left + fillW, top + barH, col);

        // thin white border
        al_draw_rectangle(left, top, right, top + barH,
                          al_map_rgb(255, 255, 255), 1);
    }
    if (PlayScene::DebugMode)
    {
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    }
}
