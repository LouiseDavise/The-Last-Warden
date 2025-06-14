#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <random>
#include <limits>

#include "Player/Player.hpp"
#include "Player/Spearman.hpp"
#include "Player/Mage.hpp"
#include "Player/Archer.hpp"
#include "Weapon/Weapon.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Weapon/WandWeapon.hpp"
#include "Weapon/BowWeapon.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Collider.hpp"
#include "PlayScene.hpp"
#include "Structure/Offense/BowTower.hpp"
#include "Structure/Defense/BasicWall.hpp"
#include "Structure/Defense/Bonfire.hpp"
#include "Structure/StructureButton.hpp"
#include "Structure/Defense/SmashBone.hpp"
#include "Structure/Defense/Axe.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Cheat.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "player_data.h"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/GreenSlime.hpp"
#include "Enemy/ToxicSlime.hpp"
#include "Enemy/LavaSlime.hpp"
#include "Enemy/Orc.hpp"
#include "Enemy/HighOrc.hpp"
#include "Player/Companion.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {
    Engine::Point(-1, 0),
    Engine::Point(1, 0),
    Engine::Point(0, -1),
    Engine::Point(0, 1),
    Engine::Point(-1, -1),
    Engine::Point(1, -1),
    Engine::Point(-1, 1),
    Engine::Point(1, 1),
};
int PlayScene::MapWidth, PlayScene::MapHeight;
const int PlayScene::BlockSize = 64;
const std::vector<int> PlayScene::CheatCode = {
    ALLEGRO_KEY_HOME, ALLEGRO_KEY_PGUP, ALLEGRO_KEY_PGDN, ALLEGRO_KEY_END};
Engine::Point PlayScene::GetClientSize()
{
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}

void PlayScene::Initialize()
{
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    SpeedMult = 1;
    matchTime = 0;
    isNight = false;
    fadingToScore = false;
    money = 1000;
    paused = false;

    AddNewObject(TileMapGroup = new Group("TileMapGroup"));
    AddNewObject(GroundEffectGroup = new Group("GroundEffectGroup"));
    AddNewObject(DebugIndicatorGroup = new Group("DebugGroup"));
    AddNewObject(ProjectileGroup = new Group("ProjectileGroup"));
    AddNewObject(EnemyGroup = new Group("EnemyGroup"));
    AddNewObject(StructureGroup = new Group("StructureGroup"));
    AddNewObject(PlayerGroup = new Group("PlayerGroup"));
    AddNewObject(WeaponGroup = new Group("WeaponGroup"));
    AddNewObject(EffectGroup = new Group("EffectGroup"));
    AddNewControlObject(UIGroup = new Group("UIGroup"));
    AddNewControlObject(PanelGroup = new Group("PanelGroup"));

    std::ifstream fin(mapFile);
    if (!fin.is_open())
    {
        std::cerr << "Failed to open map file\n";
        return;
    }
    std::string line;
    int scanWidth = 0, scanHeight = 0;
    while (std::getline(fin, line))
    {
        if ((int)line.length() > scanWidth)
            scanWidth = line.length();
        scanHeight++;
    }
    fin.close();
    MapWidth = scanWidth;
    MapHeight = scanHeight;
    MapWidth = scanWidth;
    MapHeight = scanHeight;
    ReadMap();
    float centerX = MapWidth * BlockSize / 2.0f;
    float centerY = ((MapHeight + 3) * BlockSize / 2.0f);

    if (isTwoPlayer)
    {
        companion = new Companion(centerX + 64, centerY);
        PlayerGroup->AddNewObject(companion);
    }

    Player *player = nullptr;
    if (std::string(heroType) == "SPEARMAN")
    {
        player = new Spearman(centerX, centerY);
    }
    else if (std::string(heroType) == "MAGE")
    {
        player = new Mage(centerX, centerY);
    }
    else if (std::string(heroType) == "ARCHER")
    {
        player = new Archer(centerX, centerY);
    }
    else
    {
        player = new Spearman(centerX, centerY);
    }

    PlayerGroup->AddNewObject(player);

    // Calculate distances from player
    // Use BFS to fill mapDistance (already in your code, assumed)
    UpdateBFSFromPlayer();
    GenerateFlowField();
    LoadEnemyWaves(waveFile);

    int w = al_get_display_width(al_get_current_display());
    int h = al_get_display_height(al_get_current_display());

    preview = nullptr;
    TargetTile = new Engine::Image("UI/target.png", 0, 0);
    TargetTile->Visible = false;
    UIGroup->AddNewObject(TargetTile);

    StructurePanel = new Engine::Image("UI/structure_bar.png", w / 2 - 332, h - 120, 664, 120, 0, 0);
    StructurePanel->Visible = true;
    PanelGroup->AddNewObject(StructurePanel);

    std::string moneyStr = std::to_string(money);
    totalCoin = new Engine::Label(
        moneyStr, "pirulen.ttf", 16, w / 2 - 37, h - 119);
    totalCoin->Color = al_map_rgb(255, 255, 255);
    totalCoinIcon = new Engine::Image("UI/coin-icon.png", w / 2 + 15, h - 120, 24, 24);
    AddNewObject(totalCoinIcon);
    AddNewObject(totalCoin);

    // lifeTextLabel = new Engine::Label("", "pirulen.ttf", 20, 1375 + 100,  108 + 15, 255,255,255,255, 0.5f, 0.5f);
    // UIGroup->AddNewObject(lifeTextLabel);
    ConstructUI();
    UITimerLabel = new Engine::Label("00:00:00", "pirulen.ttf", 30, al_get_display_width(al_get_current_display()) / 2, 20, 255, 255, 255, 255, 0.5f, 0.0f);
    UIGroup->AddNewObject(UITimerLabel);

    int screenW = al_get_display_width(al_get_current_display());
    int screenH = al_get_display_height(al_get_current_display());
    darknessOverlay = al_create_bitmap(screenW, screenH);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
}

void PlayScene::Terminate()
{
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void PlayScene::Update(float deltaTime)
{
    UIGroup->Update(deltaTime);
    if (paused)
    {
        return;
    }
    Player *player = GetPlayer();
    if (player)
    {
        int gx = static_cast<int>(player->Position.x) / BlockSize;
        int gy = static_cast<int>(player->Position.y) / BlockSize;

        if (gx != lastPlayerGrid.x || gy != lastPlayerGrid.y)
        {
            lastPlayerGrid = Engine::Point(gx, gy);
            UpdateBFSFromPlayer();
            GenerateFlowField();
        }
    }

    matchTime += deltaTime;
    nightCycleTimer += deltaTime;
    if (nightCycleTimer >= 100.0f)
    {
        isNight = !isNight;
        nightCycleTimer = 0.0f;
    }

    // WIN condition
    if (!fadingToScore && enemyWaves.empty())
    {
        bool allEnemiesDead = true;
        for (auto *obj : EnemyGroup->GetObjects())
        {
            Enemy *enemy = dynamic_cast<Enemy *>(obj);
            if (enemy->GetHP() > 0)
            {
                allEnemiesDead = false;
                break;
            }
        }
        if (allEnemiesDead)
        {
            fadingToScore = true;
            fadeTimer = 0;
        }
    }

    // LOSE condition
    if (player && player->isDead && !fadingToScore)
    {
        fadingToScore = true;
        fadeTimer = 0;
    }

    if (fadingToScore)
    {
        fadeTimer += deltaTime;
        if (fadeTimer >= fadeDuration)
        {
            Engine::GameEngine::GetInstance().ChangeScene("score-scene");
            return;
        }
    }

    while (!enemyWaves.empty() && matchTime >= enemyWaves.front().timestamp)
    {
        SpawnEnemy(enemyWaves.front());
        waveCount = (totalWaveSpawned / 10) + 1;
        totalWaveSpawned++;
        enemyWaves.pop();
    }

    for (int i = 0; i < SpeedMult; i++)
    {
        IScene::Update(deltaTime);
        ticks += deltaTime;
    }

    if (preview)
    {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        if (!preview->IsSmashBone())
            preview->Update(deltaTime);
    }

    auto screenSize = Engine::GameEngine::GetInstance().GetScreenSize();

    camera.x = player->Position.x - screenSize.x / 2;
    camera.y = player->Position.y - screenSize.y / 2;
    // Clamp camera so it doesn't go outside the map
    float mapWidthInPixels = MapWidth * BlockSize;
    float mapHeightInPixels = (MapHeight + 3) * BlockSize;
    float mapWidthPx = MapWidth * BlockSize;
    float mapHeightPx = (MapHeight + 3) * BlockSize;
    camera.x = std::max(0.0f, std::min(camera.x, std::max(0.0f, mapWidthPx - screenSize.x)));
    camera.y = std::max(0.0f, std::min(camera.y, std::max(0.0f, mapHeightPx - screenSize.y)));

    int totalSeconds = static_cast<int>(matchTime);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);
    UITimerLabel->Text = buffer;

    if (!activeCompanion)
    {
        for (auto *obj : PlayerGroup->GetObjects())
        {
            auto *comp = dynamic_cast<Companion *>(obj);
            if (comp)
            {
                activeCompanion = std::shared_ptr<Companion>(comp, [](Companion *) {}); // non-owning reference
                break;
            }
        }
    }
}

void PlayScene::Draw() const
{
    auto screenSize = Engine::GameEngine::GetInstance().GetScreenSize();
    int left = std::max(0, static_cast<int>(camera.x) / BlockSize - 1);
    int right = std::min(MapWidth, static_cast<int>(camera.x + screenSize.x) / BlockSize + 2);
    int top = std::max(0, static_cast<int>(camera.y) / BlockSize - 1);
    int bottom = std::min(MapHeight, static_cast<int>(camera.y + screenSize.y) / BlockSize + 2);
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_translate_transform(&transform, -camera.x, -camera.y);
    al_use_transform(&transform);
    TileMapGroup->DrawCulled(camera.x, camera.y, screenSize.x, screenSize.y);
    GroundEffectGroup->Draw();
    StructureGroup->DrawCulled(camera.x, camera.y, screenSize.x, screenSize.y);
    EnemyGroup->DrawCulled(camera.x, camera.y, screenSize.x, screenSize.y);
    PlayerGroup->Draw();
    WeaponGroup->Draw();
    ProjectileGroup->DrawCulled(camera.x, camera.y, screenSize.x, screenSize.y);
    EffectGroup->Draw();

    al_identity_transform(&transform);
    al_use_transform(&transform);

    DrawNightTime();
    Player *player = GetPlayer();
    if (player)
    {
        float hpRatio = player->GetHP() / 100.0f;
        float barWidth = 200;
        float barHeight = 20;
        float barX = 95;
        float barY = 35;

        al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(60, 60, 60));

        ALLEGRO_COLOR hpColor = al_map_rgb(255 * (1 - hpRatio), 255 * hpRatio, 0);
        al_draw_filled_rectangle(barX, barY, barX + barWidth * hpRatio, barY + barHeight, hpColor);

        al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(28, 15, 0), 1);
    }

    Spearman *spearman = dynamic_cast<Spearman *>(player);
    if (spearman)
    {
        SpearWeapon *spear = spearman->GetSpear();
        if (spear)
        {
            float cooldownRatio = spear->GetCooldownPercent();
            int usedQuota = spear->GetMaxQuota() - spear->GetQuota();

            float barWidth = 200;
            float barHeight = 16;
            float barX = 95;
            float barY = 60;

            al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(40, 40, 40));

            ALLEGRO_COLOR barColor;
            if (spear->IsCoolingDown())
                barColor = al_map_rgb(100, 200, 255);
            else
                barColor = al_map_rgb(255, 255, 0);

            float fillRatio = spear->IsCoolingDown()
                                  ? cooldownRatio
                                  : (1 - usedQuota / (float)spear->GetMaxQuota());

            al_draw_filled_rectangle(barX, barY, barX + barWidth * fillRatio, barY + barHeight, barColor);
            al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(255, 255, 255), 1);
        }
    }

    Mage *mage = dynamic_cast<Mage *>(player);
    if (mage)
    {
        WandWeapon *wand = mage->GetWand();
        if (wand)
        {
            float cooldownRatio = wand->GetCooldownPercent();
            int usedQuota = wand->GetMaxQuota() - wand->GetQuota();

            float barWidth = 200;
            float barHeight = 16;
            float barX = 95;
            float barY = 60;

            al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(30, 30, 60));

            ALLEGRO_COLOR barColor;
            if (wand->IsCoolingDown())
                barColor = al_map_rgb(100, 200, 255);
            else
                barColor = al_map_rgb(255, 255, 0);

            float fillRatio = wand->IsCoolingDown()
                                  ? cooldownRatio
                                  : (1 - usedQuota / (float)wand->GetMaxQuota());

            al_draw_filled_rectangle(barX, barY, barX + barWidth * fillRatio, barY + barHeight, barColor);
            al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(255, 255, 255), 1);
        }
    }

    Archer *archer = dynamic_cast<Archer *>(player);
    if (archer)
    {
        BowWeapon *bow = archer->GetBow();
        if (bow)
        {
            float cooldownRatio = bow->GetCooldownPercent();
            int usedQuota = bow->GetMaxQuota() - bow->GetQuota();

            float barWidth = 200;
            float barHeight = 16;
            float barX = 95;
            float barY = 60;

            al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(60, 30, 30));

            ALLEGRO_COLOR barColor;
            if (bow->IsCoolingDown())
                barColor = al_map_rgb(255, 120, 120);
            else
                barColor = al_map_rgb(255, 255, 0);

            float fillRatio = bow->IsCoolingDown()
                                  ? cooldownRatio
                                  : (1 - usedQuota / (float)bow->GetMaxQuota());

            al_draw_filled_rectangle(barX, barY, barX + barWidth * fillRatio, barY + barHeight, barColor);
            al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(255, 255, 255), 1);
        }
    }

    // Profile Border
    // float centerX = 20 + 32;
    // float centerY = 20 + 32;
    // float radius = 32;
    // al_draw_circle(centerX, centerY, radius + 1.5f, al_map_rgb(255, 255, 255), 5.0f);
    UIGroup->Draw();
    PanelGroup->Draw();

    totalCoin->Text = std::to_string(money);
    if (totalCoin->Visible)
        totalCoin->Draw();
    if (totalCoinIcon->Visible)
        totalCoinIcon->Draw();
    if (fadingToScore)
    {
        float alpha = std::min(fadeTimer / fadeDuration, 1.0f);
        ALLEGRO_COLOR fadeColor = al_map_rgba_f(0, 0, 0, alpha);
        al_draw_filled_rectangle(0, 0, al_get_display_width(al_get_current_display()), al_get_display_height(al_get_current_display()), fadeColor);
    }

    if (activeCompanion)
    {
        std::string type = std::string(companionType);
        const float x = 95;
        const float y = 90; // Right below weapon cooldown
        const float width = 200;
        const float height = 8;

        float ratio = 1.0f;
        ALLEGRO_COLOR fillColor = al_map_rgb(255, 255, 255); // default white

        if (type == "COMP1")
        {
            ratio = activeCompanion->IsWispSkillReady() ? 1.0f : activeCompanion->GetWispCooldownProgress();
            fillColor = activeCompanion->IsWispSkillReady() ? al_map_rgb(100, 255, 100) : // green when ready
                            al_map_rgb(255, 150, 0);                                      // orange when recharging
        }
        else if (type == "COMP2")
        {
            ratio = activeCompanion->IsMochiSkillReady() ? 1.0f : activeCompanion->GetMochiCooldownProgress();
            fillColor = activeCompanion->IsMochiSkillReady() ? al_map_rgb(0, 255, 150) : // teal green when ready
                            al_map_rgb(255, 100, 100);                                   // reddish when recharging
        }
        else if (type == "COMP3")
        {
            ratio = activeCompanion->IsZukoSkillReady() ? 1.0f : activeCompanion->GetzukoSkillCooldownProgress();
            fillColor = activeCompanion->IsZukoSkillReady() ? al_map_rgb(0, 255, 150) : // teal green when ready
                            al_map_rgb(255, 200, 100);                                  // reddish when recharging
        }

        // Background
        al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(40, 40, 40));
        // Fill
        al_draw_filled_rectangle(x, y, x + width * ratio, y + height, fillColor);
        // Border
        al_draw_rectangle(x, y, x + width, y + height, al_map_rgb(255, 255, 255), 2);
    }
}

void PlayScene::OnMouseDown(int button, int mx, int my)
{
    UIGroup->OnMouseDown(button, mx, my);
    if ((button & 1))
    {
        if (pauseButton->Visible &&
            mx >= pauseButton->Position.x && mx <= pauseButton->Position.x + 55 &&
            my >= pauseButton->Position.y && my <= pauseButton->Position.y + 55)
        {
            paused = true;
            pauseButton->Visible = false;
            playButton->Visible = true;
            return;
        }

        if (playButton->Visible &&
            mx >= pauseButton->Position.x && mx <= pauseButton->Position.x + 55 &&
            my >= pauseButton->Position.y && my <= pauseButton->Position.y + 55)
        {
            paused = false;
            playButton->Visible = false;
            pauseButton->Visible = true;
            return;
        }

        if (paused)
            return; // Don't allow game actions while paused
    }

    Player *player = GetPlayer();
    if ((button & 1) && !TargetTile->Visible && preview)
        if ((button & 1) && !TargetTile->Visible && preview)
        {
            // Cancel construct.
            UIGroup->RemoveObject(preview->GetObjectIterator());
            preview = nullptr;
        }
    IScene::OnMouseDown(button, mx, my);
    if (!IsMouseOverUI(mx, my))
        player->OnMouseDown(button, mx, my);
}

void PlayScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);
    int x = mx / BlockSize;
    int y = my / BlockSize;

    if (preview && preview->IsSmashBone())
    {
        int gx = (mx + camera.x) / BlockSize;
        int gy = (my + camera.y) / BlockSize;
        preview->Position = Engine::Point{(float)gx, (float)gy};
        Structure *tgt = GetStructureAt(gx, gy);
        if (tgt != highlightedStructure)
        {
            if (highlightedStructure)
                highlightedStructure->Tint = al_map_rgba(255, 255, 255, 255);
            if (tgt)
                tgt->Tint = al_map_rgba(255, 60, 60, 255); // bright red
            highlightedStructure = tgt;
        }
        TargetTile->Visible = false;
        return;
    }

    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        TargetTile->Visible = false;
        TargetTile->Visible = false;
        return;
    }
    TargetTile->Visible = true;
    TargetTile->Position.x = x * BlockSize;
    TargetTile->Position.y = y * BlockSize;
    TargetTile->Visible = true;
    TargetTile->Position.x = x * BlockSize;
    TargetTile->Position.y = y * BlockSize;
}

void PlayScene::OnMouseUp(int button, int mx, int my)
{
    if (IsMouseOverUI(mx, my))
        return;
    IScene::OnMouseUp(button, mx, my);
    int gx = (mx + camera.x) / BlockSize;
    int gy = (my + camera.y) / BlockSize;
    if (preview && preview->IsSmashBone())
    {
        if (button & 1)
        {
            Structure *s = GetStructureAt(gx, gy);
            if (s)
            {
                StructureGroup->RemoveObject(s->GetObjectIterator());
                AudioHelper::PlaySample("win.wav");
                mapState[gy][gx] = TILE_WALKABLE;
            }
            else
            {
                UIGroup->RemoveObject(preview->GetObjectIterator());
                preview = nullptr;
            }
        }
        if (highlightedStructure)
        {
            highlightedStructure->Tint = al_map_rgba(255, 255, 255, 255);
            highlightedStructure = nullptr;
        }
        return;
    }

    if (preview && preview->IsAxe())
    {
        if (button & 1)
        {
            if (mapState[gy][gx] == TILE_PROPERTY)
            {
                // Chop tree: turn into TILE_WALKABLE and replace image
                static std::default_random_engine rng((std::random_device())());
                std::uniform_int_distribution<int> baseDist(1, 9);
                int baseIdx = baseDist(rng);
                std::string basePath = "Tileset/base/image1x" + std::to_string(baseIdx) + ".png";

                mapState[gy][gx] = TILE_WALKABLE;

                // Add new base tile
                TileMapGroup->AddNewObject(new Engine::Image(basePath, gx * BlockSize, gy * BlockSize, BlockSize, BlockSize));
                AudioHelper::PlaySample("win.wav");
            }
            else
            {
                UIGroup->RemoveObject(preview->GetObjectIterator());
                preview = nullptr;
            }
        }
        return;
    }

    if (!TargetTile->Visible)
        return;
    if (button & 1)
    {
        if (mapState[gy][gx] != TILE_OCCUPIED)
        {
            if (!preview)
                return;
            if (!CheckSpaceValid(gx, gy))
            {
                Engine::Sprite *sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("UI/target-invalid.png", 1, gx * BlockSize + BlockSize / 2, gy * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            AddMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            preview->Position.x = gx * BlockSize + BlockSize / 2;
            preview->Position.y = gy * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            preview->occupyX = gx;
            preview->occupyY = gy;
            StructureGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);
            // Remove Preview.
            preview = nullptr;

            mapState[gy][gx] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode)
{
    Player *player = GetPlayer();
    IScene::OnKeyDown(keyCode);
    for (auto *obj : PlayerGroup->GetObjects())
    {
        if (Player *p = dynamic_cast<Player *>(obj))
        {
            p->OnKeyDown(keyCode);
        }
    }

    if (companion)
        companion->OnKeyDown(keyCode);

    if (keyCode == ALLEGRO_KEY_TAB)
    {
        DebugMode = !DebugMode;
    }
    else
    {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > CheatCode.size())
            if (keyStrokes.size() > CheatCode.size())
                keyStrokes.pop_front();
    }
    if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    else if (keyCode == ALLEGRO_KEY_ESCAPE && preview && preview->IsSmashBone())
    {
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
        return;
    }
    else if (keyCode == ALLEGRO_KEY_H)
    {
        PanelVisible = !PanelVisible;
        totalCoin->Visible = !totalCoin->Visible;
        totalCoinIcon->Visible = !totalCoinIcon->Visible;
        for (auto *obj : PanelGroup->GetObjects())
        {
            obj->Visible = !obj->Visible;
        }
    }

    if (keyStrokes.size() == CheatCode.size() && std::equal(CheatCode.begin(), CheatCode.end(), keyStrokes.begin()))
    {
        EffectGroup->AddNewObject(new Cheat());
        AudioHelper::PlaySample("gun.wav");
        AddMoney(1000);
    }
}

void PlayScene::OnKeyUp(int keyCode)
{
    for (auto *obj : PlayerGroup->GetObjects())
    {
        if (Player *p = dynamic_cast<Player *>(obj))
        {
            p->OnKeyUp(keyCode);
        }
    }
    if (companion)
        companion->OnKeyUp(keyCode);
}

void PlayScene::SetMapFile(const std::string &filename)
{
    mapFile = filename;
    Engine::LOG(Engine::INFO) << "Set Map File to :  " << filename;
}

void PlayScene::SetWaveFile(const std::string &filename)
{
    waveFile = filename;
}

void PlayScene::SetTwoPlayerMode(bool twoP)
{
    isTwoPlayer = twoP;
}

void PlayScene::ReadMap()
{
    static std::default_random_engine rng((std::random_device())());
    std::uniform_int_distribution<int> baseDist(1, 9);
    std::uniform_int_distribution<int> treeDist(1, 12);

    std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    float treeChance = 0.1f;

    int dx[4] = {0, -1, 0, 1}; // up, left, down, right
    int dy[4] = {-1, 0, 1, 0};

    std::string filename = mapFile;
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Failed to open map file\n";
        return;
    }

    std::vector<std::string> mapChar(MapHeight);
    for (int i = 0; i < MapHeight; ++i)
    {
        std::getline(fin, mapChar[i]);
    }

    auto isSameRiver = [&](int cx, int cy, int nx, int ny)
    {
        if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
            return false;
        return mapChar[cy][cx] == mapChar[ny][nx];
    };

    // For center tile detection (non-edge)
    auto isRiverEdge = [&](int x, int y)
    {
        if (mapChar[y][x] != '2' && mapChar[y][x] != '3')
            return false;
        for (int d = 0; d < 4; ++d)
        {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                return true;
            if (mapChar[ny][nx] != mapChar[y][x])
                return true;
        }
        return false;
    };

    // Prepare map state
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));

    for (int i = 0; i < MapHeight; ++i)
    {
        for (int j = 0; j < MapWidth; ++j)
        {
            char tileChar = mapChar[i][j];

            // Base tile
            int baseIdx = baseDist(rng);
            std::string basePath = "Tileset/base/image1x" + std::to_string(baseIdx) + ".png";
            TileMapGroup->AddNewObject(new Engine::Image(basePath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));

            switch (tileChar)
            {
            case '0':
                mapState[i][j] = TILE_WALKABLE;
                break;

                // case '1':
                // {
                //     mapState[i][j] = TILE_WALKABLE;
                //     int treeIdx = treeDist(rng);
                //     std::string treePath = "Tileset/tree/image1x" + std::to_string(treeIdx) + ".png";
                //     TileMapGroup->AddNewObject(new Engine::Image(treePath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     break;
                // }

            case '1':
            {
                mapState[i][j] = TILE_OBSTRUCTION;

                auto isRiverLike = [&](int nx, int ny)
                {
                    if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                        return false;
                    char c = mapChar[ny][nx];
                    return c == '1' || c == '3' || c == '4';
                };

                auto isDirt = [&](int nx, int ny)
                {
                    if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                        return false;
                    return mapChar[ny][nx] == '0';
                };

                bool up = isRiverLike(j, i - 1);
                bool down = isRiverLike(j, i + 1);
                bool left = isRiverLike(j - 1, i);
                bool right = isRiverLike(j + 1, i);

                std::string riverPath;

                if (isDirt(j - 1, i) && right && !up && !down) // left is dirt
                    riverPath = "Tileset/river/image1x8.png";
                else if (isDirt(j + 1, i) && left && !up && !down) // right is dirt
                    riverPath = "Tileset/river/image1x16.png";
                else if (isDirt(j, i - 1) && !left && !right && down) // up is dirt
                    riverPath = "Tileset/river/image1x1.png";
                else if (isDirt(j, i + 1) && !left && !right && up) // down is dirt
                    riverPath = "Tileset/river/image1x3.png";

                // 4-way cross
                else if (up && down && left && right)
                    riverPath = "Tileset/river/image1x10.png"; // 4-way

                // 3-way branches
                else if (up && left && down)
                    riverPath = "Tileset/river/image1x14.png"; // top + left + down
                else if (up && left && right)
                    riverPath = "Tileset/river/image1x11.png"; // top + left + right
                else if (up && right && down)
                    riverPath = "Tileset/river/image1x6.png"; // top + right + down
                else if (left && right && down)
                    riverPath = "Tileset/river/image1x9.png"; // left + right + down

                // Corners
                else if (left && down)
                    riverPath = "Tileset/river/image1x13.png"; // left + down
                else if (right && down)
                    riverPath = "Tileset/river/image1x5.png"; // right + down
                else if (up && right)
                    riverPath = "Tileset/river/image1x17.png"; // top + right
                else if (up && left)
                    riverPath = "Tileset/river/image1x15.png"; // top + left

                // Straight paths
                else if (left && right && !up && !down)
                    riverPath = "Tileset/river/image1x12.png"; // horizontal
                else if (up && down && !left && !right)
                    riverPath = "Tileset/river/image1x2.png"; // vertical

                // Fallback
                else
                    riverPath = "Tileset/river/image1x12.png"; // center / fill

                TileMapGroup->AddNewObject(new Engine::Image(riverPath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                break;
            }

            case '2':
            {
                mapState[i][j] = TILE_OBSTRUCTION;

                auto isSameWall = [&](int cx, int cy, int nx, int ny)
                {
                    if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                        return false;
                    return mapChar[cy][cx] == '2';
                };

                bool hasUp = isSameWall(j, i, j, i - 1);
                bool hasDown = isSameWall(j, i, j, i + 1);
                bool hasLeft = isSameWall(j, i, j - 1, i);
                bool hasRight = isSameWall(j, i, j + 1, i);

                std::string wallPath;

                if (!hasUp && !hasLeft && hasDown && hasRight)
                    wallPath = "Tileset/wall/image1x1.png"; // top-left
                else if (!hasUp && !hasRight && hasDown && hasLeft)
                    wallPath = "Tileset/wall/image1x2.png"; // top-right
                else if (!hasDown && !hasLeft && hasUp && hasRight)
                    wallPath = "Tileset/wall/image1x3.png"; // bottom-left
                else if (!hasDown && !hasRight && hasUp && hasLeft)
                    wallPath = "Tileset/wall/image1x4.png"; // bottom-right

                // Edges
                else if (!hasUp)
                    wallPath = "Tileset/wall/image1x5.png"; // top edge
                else if (!hasDown)
                    wallPath = "Tileset/wall/image1x6.png"; // bottom edge
                else if (!hasLeft)
                    wallPath = "Tileset/wall/image1x7.png"; // left edge
                else if (!hasRight)
                    wallPath = "Tileset/wall/image1x8.png"; // right edge
                else
                    wallPath = "Tileset/wall/image1x7.png"; // generic wall

                TileMapGroup->AddNewObject(new Engine::Image(wallPath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                break;
            }

            case '3':
            {
                mapState[i][j] = TILE_OBSTRUCTION;

                auto isRiverLike = [&](int nx, int ny)
                {
                    if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                        return false;
                    char c = mapChar[ny][nx];
                    return c == '1' || c == '3';
                };

                bool up = isRiverLike(j, i - 1);
                bool down = isRiverLike(j, i + 1);
                bool left = isRiverLike(j - 1, i);
                bool right = isRiverLike(j + 1, i);

                std::string riverPath;

                if (isRiverEdge(j, i))
                {
                    // 4-way cross
                    if (up && down && left && right)
                        riverPath = "Tileset/river/image1x10.png";
                    // 3-way branches
                    else if (up && left && down)
                        riverPath = "Tileset/river/image1x14.png";
                    else if (up && left && right)
                        riverPath = "Tileset/river/image1x11.png";
                    else if (up && right && down)
                        riverPath = "Tileset/river/image1x6.png";
                    else if (left && right && down)
                        riverPath = "Tileset/river/image1x9.png";

                    // Corners
                    else if (!up && !left && down && right)
                        riverPath = "Tileset/river/image1x5.png"; // top-left
                    else if (!up && !right && down && left)
                        riverPath = "Tileset/river/image1x13.png"; // top-right
                    else if (!down && !left && up && right)
                        riverPath = "Tileset/river/image1x17.png"; // bottom-left
                    else if (!down && !right && up && left)
                        riverPath = "Tileset/river/image1x15.png"; // bottom-right

                    // Straight lines
                    else if (left && right && !up && !down)
                        riverPath = "Tileset/river/image1x12.png"; // horizontal
                    else if (up && down && !left && !right)
                        riverPath = "Tileset/river/image1x2.png"; // vertical

                    // Fallback
                    else
                        riverPath = "Tileset/river/image1x12.png";
                }
                else
                {
                    // Center tile fill
                    riverPath = "Tileset/river/image1x8.png";
                }

                TileMapGroup->AddNewObject(new Engine::Image(riverPath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                break;
            }
            case '4':
            {
                mapState[i][j] = TILE_BRIDGE;

                auto isRiverLike = [&](int nx, int ny)
                {
                    if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                        return false;
                    char c = mapChar[ny][nx];
                    return c == '1' || c == '3';
                };

                bool left = isRiverLike(j - 1, i);
                bool right = isRiverLike(j + 1, i);
                bool up = isRiverLike(j, i - 1);
                bool down = isRiverLike(j, i + 1);

                std::string bridgePath;

                if (left || right)
                    bridgePath = "Tileset/bridge/image1x1.png"; // horizontal bridge
                else if (up || down)
                    bridgePath = "Tileset/bridge/image1x2.png"; // vertical bridge
                else
                    bridgePath = "Tileset/bridge/image1x1.png"; // fallback

                TileMapGroup->AddNewObject(new Engine::Image(bridgePath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                break;
            }

            default:
                mapState[i][j] = TILE_WALKABLE;
                break;
            }
            if (tileChar == '0' && prob(rng) < treeChance)
            {
                mapState[i][j] = TILE_PROPERTY;
                int treeIdx = treeDist(rng);
                std::string treePath = "Tileset/tree/image1x" + std::to_string(treeIdx) + ".png";
                TileMapGroup->AddNewObject(new Engine::Image(treePath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
        }
    }
}

bool PlayScene::validLine(Engine::Point from, Engine::Point to)
{
    int x0 = from.x / BlockSize;
    int y0 = from.y / BlockSize;
    int x1 = to.x / BlockSize;
    int y1 = to.y / BlockSize;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        if (!IsWalkable(x0, y0))
            return false;
        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }

    return true;
}

void PlayScene::GenerateFlowField()
{
    // Initialize with zero vectors
    flowField = std::vector<std::vector<Engine::Point>>(MapHeight, std::vector<Engine::Point>(MapWidth, Engine::Point(0, 0)));

    for (int y = 0; y < MapHeight; ++y)
    {
        for (int x = 0; x < MapWidth; ++x)
        {
            if (mapDistance[y][x] == -1)
                continue;

            int bestDist = mapDistance[y][x];
            Engine::Point bestDir(0, 0);

            // Check 4 directions
            for (const Engine::Point &d : directions)
            {
                int nx = x + d.x;
                int ny = y + d.y;
                if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                    continue;
                if (mapDistance[ny][nx] == -1)
                    continue;
                if (mapDistance[ny][nx] < bestDist)
                {
                    bestDist = mapDistance[ny][nx];
                    bestDir = Engine::Point(d.x, d.y);
                }
            }

            if (bestDir.x != 0 || bestDir.y != 0)
            {
                float len = std::sqrt(bestDir.x * bestDir.x + bestDir.y * bestDir.y);
                flowField[y][x] = Engine::Point(bestDir.x / len, bestDir.y / len);
            }
        }
    }
}

void PlayScene::UpdateBFSFromPlayer()
{
    // Reset distance map
    mapDistance = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth, -1));

    Player *player = GetPlayer();
    if (!player)
        return;

    int px = static_cast<int>(player->Position.x) / BlockSize;
    int py = static_cast<int>(player->Position.y) / BlockSize;

    if (px < 0 || px >= MapWidth || py < 0 || py >= MapHeight)
        return;

    std::queue<std::pair<int, int>> q;
    mapDistance[py][px] = 0;
    q.emplace(px, py);

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (const auto &dir : directions)
        {
            int nx = x + dir.x;
            int ny = y + dir.y;
            if (nx < 0 || ny < 0 || nx >= MapWidth || ny >= MapHeight)
                continue;
            if (!IsWalkable(nx, ny))
                continue;
            if (mapDistance[ny][nx] != -1)
                continue;

            if (abs(dir.x) == 1 && abs(dir.y) == 1)
            {
                if (!IsWalkable(x + dir.x, y) || !IsWalkable(x, y + dir.y))
                    continue;
            }

            mapDistance[ny][nx] = mapDistance[y][x] + 1;
            q.emplace(nx, ny);
        }
    }
}

bool PlayScene::IsWalkable(int x, int y)
{
    return mapState[y][x] != TILE_OBSTRUCTION;
}

void PlayScene::LoadEnemyWaves(const std::string &filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Failed to open enemy file\n";
        return;
    }

    int type;
    float timestamp, count;
    while (fin >> timestamp >> type >> count)
    {
        enemyWaves.push(EnemyWave{timestamp, type, count});
    }
}

void PlayScene::SpawnEnemy(const EnemyWave &wave)
{
    static std::default_random_engine rng((std::random_device())());
    std::uniform_int_distribution<int> edgeDist(0, 3); // 0: top, 1: bottom, 2: left, 3: right
    std::uniform_int_distribution<int> xDist(2, MapWidth - 3);
    std::uniform_int_distribution<int> yDist(2, MapHeight - 3);

    for (int i = 0; i < wave.count; ++i)
    {
        int gx = 0, gy = 0;
        bool valid = false;

        // Try multiple times in case edge tile is not walkable
        for (int attempt = 0; attempt < 100 && !valid; ++attempt)
        {
            int edge = edgeDist(rng);
            switch (edge)
            {
            case 0: // Top edge
                gx = xDist(rng);
                gy = 2;
                break;
            case 1: // Bottom edge
                gx = xDist(rng);
                gy = MapHeight - 3;
                break;
            case 2: // Left edge
                gx = 2;
                gy = yDist(rng);
                break;
            case 3: // Right edge
                gx = MapWidth - 3;
                gy = yDist(rng);
                break;
            }
            valid = (mapState[gy][gx] == TILE_WALKABLE);
            Engine::LOG(Engine::INFO) << "ATTEMPT (" << gy << "),(" << gx << ") : result->" << valid;
            if (valid)
                break;
        }
        if (!valid)
            continue; // skip this spawn if no valid edge tile found

        float spawnX = gx * BlockSize + BlockSize / 2;
        float spawnY = gy * BlockSize + BlockSize / 2;

        Enemy *toSpawn = nullptr;
        switch (wave.type)
        {
        case 1:
            toSpawn = new GreenSlime(spawnX, spawnY);
            break;
        case 2:
            toSpawn = new ToxicSlime(spawnX, spawnY);
            break;
        case 3:
            toSpawn = new LavaSlime(spawnX, spawnY);
            break;
        case 4:
            toSpawn = new Orc(spawnX, spawnY);
            break;
        case 5:
            toSpawn = new HighOrc(spawnX, spawnY);
            break;
        }

        if (!toSpawn)
            continue;

        if (!mapDistance.empty() && mapDistance[gy][gx] != -1)
        {
            toSpawn->UpdatePath(mapDistance);
        }

        EnemyGroup->AddNewObject(toSpawn);
    }
}

void PlayScene::AddMoney(int amount)
{
    money += amount;
    totalMoneyEarned += amount;
}

bool PlayScene::SpendMoney(int amount)
{
    if (money >= amount)
    {
        money -= amount;
        totalMoneySpent += amount;
        return true;
    }
    return false;
}

void PlayScene::ConstructUI()
{
    std::string profileImagePath;
    if (std::string(heroType) == "MAGE")
        profileImagePath = "Characters/Mage/mage-profile.png";
    else if (std::string(heroType) == "ARCHER")
        profileImagePath = "Characters/Archer/archer-profile.png";
    else if (std::string(heroType) == "SPEARMAN")
        profileImagePath = "Characters/Spearman/spearman-profile.png";

    playerProfileImage = new Engine::Image(profileImagePath, 20, 20, 64, 64);
    UIGroup->AddNewObject(playerProfileImage);

    int screenW = al_get_display_width(al_get_current_display());

    pauseButton = new Engine::Image("UI/pause-button-transparant.png", screenW - 80, 20, 55, 55);
    playButton = new Engine::Image("UI/play-button-transparant.png", screenW - 80, 20, 55, 55);
    playButton->Visible = false;

    pauseButton->Visible = true;
    UIGroup->AddNewObject(pauseButton);
    UIGroup->AddNewObject(playButton);

    int w = al_get_display_width(al_get_current_display());
    int h = al_get_display_height(al_get_current_display());
    struct BtnInfo
    {
        int x, y;
        int price;
        int btnId;
        const char *sprite;
        const char *base;
        float yOff;
    };
    std::vector<BtnInfo> btns = {
        {w / 2 - 332 + 8 + 72 * 0, h - 94, BowTower::Price, 1, "Structures/BowTower.png", "Structures/tower-base.png", 0},
        {w / 2 - 332 + 8 + 72 * 1, h - 94, BasicWall::Price, 2, "Structures/BasicWall.png", "Structures/blank.png", 3},
        {w / 2 - 332 + 8 + 72 * 2, h - 94, Bonfire::Price, 3, "Structures/Bonfire.png", "Structures/blank.png", 8}
    };

    for (auto &b : btns)
    {
        StructureButton *btn = new StructureButton("UI/structurebtn.png", "UI/structurebtn_hovered.png",
                                                   Engine::Sprite(b.base, b.x + 36, b.y + 38, 0, 0, 0.5, 0.5),
                                                   Engine::Sprite(b.sprite, b.x + 36, b.y + 30 + b.yOff, 0, 0, 0.5, 0.5),
                                                   b.x, b.y, b.price);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, b.btnId));
        PanelGroup->AddNewControlObject(btn);

        float pricePanelCenter = b.x + 37;
        ALLEGRO_FONT *font = al_load_ttf_font("Resource/fonts/pirulen.ttf", 16, 0);
        std::string priceStr = std::to_string(b.price);
        float textW = al_get_text_width(font, priceStr.c_str());

        auto *priceLbl = new Engine::Label(
            priceStr, "pirulen.ttf", 16, pricePanelCenter - (textW + 20) / 2, b.y + 73);
        priceLbl->Color = al_map_rgb(255, 255, 255);

        PanelGroup->AddNewObject(priceLbl);
        Engine::Image *coinImg = new Engine::Image("UI/coin-icon.png", pricePanelCenter - (textW + 20) / 2 + textW, b.y + 71, 24, 24);
        PanelGroup->AddNewObject(coinImg);
    }

    // Axe UI
    StructureButton *btn = new StructureButton("UI/structurebtn.png", "UI/structurebtn_hovered.png",
                                               Engine::Sprite("Structures/blank.png", w / 2 - 332 + 8 + 72 * 7 + 36, h - 94 + 38, 0, 0, 0.5, 0.5),
                                               Engine::Sprite("Structures/Axe.png", w / 2 - 332 + 8 + 72 * 7 + 36, h - 90 + 30, 0, 0, 0.5, 0.5),
                                               w / 2 - 332 + 8 + 72 * 7, h - 94, Axe::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, -1));
    PanelGroup->AddNewControlObject(btn);
    float pricePanelCenter = w / 2 - 332 + 8 + 72 * 7 + 37;
    ALLEGRO_FONT *font = al_load_ttf_font("Resource/fonts/pirulen.ttf", 16, 0);
    std::string priceStr = std::to_string(Axe::Price);
    float textW = al_get_text_width(font, priceStr.c_str());

    auto *priceLbl = new Engine::Label(
        priceStr, "pirulen.ttf", 16, pricePanelCenter - (textW + 20) / 2, h - 94 + 73);
    priceLbl->Color = al_map_rgb(255, 255, 255);
    PanelGroup->AddNewObject(priceLbl);
    Engine::Image *coinImg = new Engine::Image("UI/coin-icon.png", pricePanelCenter - (textW + 20) / 2 + textW, h - 94 + 71, 24, 24);
    PanelGroup->AddNewObject(coinImg);

    // SmashBone UI
    btn = new StructureButton("UI/structurebtn.png", "UI/structurebtn_hovered.png",
                              Engine::Sprite("Structures/blank.png", w / 2 - 332 + 8 + 72 * 8 + 36, h - 94 + 38, 0, 0, 0.5, 0.5),
                              Engine::Sprite("Structures/SmashBone.png", w / 2 - 332 + 8 + 72 * 8 + 36, h - 90 + 30, 0, 0, 0.5, 0.5),
                              w / 2 - 332 + 8 + 72 * 8, h - 94, SmashBone::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    PanelGroup->AddNewControlObject(btn);
    priceLbl = new Engine::Label(
        "-", "pirulen.ttf", 16, w / 2 - 332 + 8 + 72 * 8 + 32, h - 90 + 71);
    priceLbl->Color = al_map_rgb(255, 255, 255);
    PanelGroup->AddNewObject(priceLbl);

    int screenWidth = Engine::GameEngine::GetInstance().GetScreenSize().x;

    // Home Button (top-right)
    homeButton = new Engine::ImageButton("UI/home-button-transparant.png", "UI/home-button-transparant.png", screenWidth - 70, 20, 55, 55);
    homeButton->SetOnClickCallback([this]()
                                   {
    paused = true; 
    if (!homeConfirmBox) return;

    homeConfirmBox->Visible = true;
    homeConfirmText1->Visible = true;
    homeConfirmText2->Visible = true;
    homeWarning1->Visible = true;
    homeWarning2->Visible = true;
    homeConfirmBtn->Visible = true;
    homeConfirmBtn->Enabled = true;
    homeCancelBtn->Visible = true;
    homeConfirmLabel->Visible = true;
    homeCancelLabel->Visible = true; });
    UIGroup->AddNewControlObject(homeButton);

    // Move pause button to left of home
    if (pauseButton)
        pauseButton->Position.x = screenWidth - 140;
    if (playButton)
        playButton->Position.x = screenWidth - 140;

    // Confirm UI
    homeConfirmBox = new Engine::Image("UI/confirm-box.png", screenWidth / 2 - 300, 300, 600, 400);
    homeConfirmBox->Visible = false;
    UIGroup->AddNewObject(homeConfirmBox);

    homeConfirmText1 = new Engine::Label(
        "Are you sure you want to return to the main menu?",
        "RealwoodRegular.otf", 30, screenWidth / 2, 380, 255, 255, 255, 255, 0.5, 0.5);
    homeConfirmText2 = new Engine::Label(
        "Your progress will be automatically recorded in the leaderboard.",
        "RealwoodRegular.otf", 22, screenWidth / 2, 420, 255, 255, 255, 255, 0.5, 0.5);

    homeConfirmText1->Visible = false;
    homeConfirmText2->Visible = false;

    UIGroup->AddNewObject(homeConfirmText1);
    UIGroup->AddNewObject(homeConfirmText2);

    homeWarning1 = new Engine::Label("Leaving now will end this game session.", "RealwoodRegular.otf", 20, screenWidth / 2, 470, 255, 100, 100, 255, 0.5, 0.5);
    homeWarning1->Visible = false;
    UIGroup->AddNewObject(homeWarning1);

    homeWarning2 = new Engine::Label("Do you really want to go home?", "RealwoodRegular.otf", 20, screenWidth / 2, 495, 255, 100, 100, 255, 0.5, 0.5);
    homeWarning2->Visible = false;
    UIGroup->AddNewObject(homeWarning2);

    // Confirm
    homeConfirmBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", screenWidth / 2 - 160, 550, 140, 65);
    homeConfirmBtn->SetOnClickCallback([]()
                                       { Engine::GameEngine::GetInstance().ChangeScene("score-scene"); });

    homeConfirmBtn->Visible = false;
    homeConfirmBtn->Enabled = false;
    UIGroup->AddNewControlObject(homeConfirmBtn);

    homeConfirmLabel = new Engine::Label("Confirm", "pirulen.ttf", 17, screenWidth / 2 - 90, 580, 255, 255, 255, 255, 0.5, 0.5);
    homeConfirmLabel->Visible = false;
    UIGroup->AddNewObject(homeConfirmLabel);

    // Cancel
    homeCancelBtn = new Engine::ImageButton("UI/button.png", "UI/button-transparant.png", screenWidth / 2 + 40, 550, 140, 65);
    homeCancelBtn->SetOnClickCallback([this]()
                                      {
    paused = false; 
    homeConfirmBox->Visible = false;
    homeConfirmText1->Visible = false;
    homeConfirmText2->Visible = false;
    homeWarning1->Visible = false;
    homeWarning2->Visible = false;
    homeConfirmBtn->Visible = false;
    homeConfirmBtn->Enabled = false;
    homeCancelBtn->Visible = false;
    homeConfirmLabel->Visible = false;
    homeCancelLabel->Visible = false; });
    homeCancelBtn->Visible = false;
    UIGroup->AddNewControlObject(homeCancelBtn);

    homeCancelLabel = new Engine::Label("Cancel", "pirulen.ttf", 17, screenWidth / 2 + 110, 580, 255, 255, 255, 255, 0.5, 0.5);
    homeCancelLabel->Visible = false;
    UIGroup->AddNewObject(homeCancelLabel);
}

void PlayScene::UIBtnClicked(int id)
{
    if (preview)
    {
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }

    if (PanelVisible)
        switch (id)
        {
        case -1:
            preview = new Axe(0, 0);
            break;
        case 0:
            preview = new SmashBone(0, 0);
            break;
        case 1:
            if (money >= BowTower::Price)
                preview = new BowTower(0, 0);
            break;
        case 2:
            if (money >= BasicWall::Price)
                preview = new BasicWall(0, 0);
            break;
        case 3:
            if (money >= Bonfire::Price)
                preview = new Bonfire(0, 0);
            break;
        }
    if (!preview)
        return;
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y)
{
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;

    if (mapState[y][x] == TILE_OCCUPIED || mapState[y][x] == TILE_OBSTRUCTION || mapState[y][x] == TILE_PROPERTY)
        return false;

    // Only allow placement on TILE_WALKABLE or TILE_BRIDGE
    if (mapState[y][x] != TILE_WALKABLE && mapState[y][x] != TILE_BRIDGE)
        return false;

    return true;
}

Structure *PlayScene::GetStructureAt(int gx, int gy)
{
    for (auto *obj : StructureGroup->GetObjects())
    {
        auto *s = dynamic_cast<Structure *>(obj);
        if (!s)
            continue;
        int sx = int(s->Position.x) / BlockSize;
        int sy = int(s->Position.y) / BlockSize;
        if (sx == gx && sy == gy)
            return s;
    }
    return nullptr;
}

void PlayScene::DrawNightTime() const
{
    if (!isNight || !darknessOverlay)
        return;

    Player *player = GetPlayer();
    if (!player)
        return;

    Engine::Point screenCenter = player->Position - camera;
    Engine::Point mousePos = Engine::GameEngine::GetInstance().GetMousePosition();

    int screenW = al_get_display_width(al_get_current_display());
    int screenH = al_get_display_height(al_get_current_display());

    ALLEGRO_BITMAP *oldTarget = al_get_target_bitmap();
    al_set_target_bitmap(darknessOverlay);

    // Fill screen with almost-black (multiplicative blackout)
    al_clear_to_color(al_map_rgb(2, 2, 8));

    if(isTwoPlayer) {
        Player* companion = GetCompanion();
        if(!companion) return;
        Engine::Point screenCenter = companion->Position - camera;
        int layers = 4;
        float baseRadius = 130.0f;
        for (int i = 0; i < layers; ++i)
        {
            float r = baseRadius - i * 10.0f;
            int brightness = 50 + i * 40;
            brightness = std::min(brightness, 255);
            al_draw_filled_circle(screenCenter.x, screenCenter.y, r, al_map_rgb(brightness * 0.425, brightness * 0.425, brightness));
        }
    }

    int layers = 4;
    float baseRadius = 130.0f;
    for (int i = 0; i < layers; ++i)
    {
        float r = baseRadius - i * 10.0f;
        int brightness = 50 + i * 40;
        brightness = std::min(brightness, 255);
        al_draw_filled_circle(screenCenter.x, screenCenter.y, r, al_map_rgb(brightness * 0.425, brightness * 0.425, brightness));
    }

    float dx = mousePos.x - screenCenter.x;
    float dy = mousePos.y - screenCenter.y;
    float angle = std::atan2(dy, dx);
    float coneLength = 500;
    float coneAngle = ALLEGRO_PI / 15.0f;

    float x1 = screenCenter.x + coneLength * std::cos(angle - coneAngle);
    float y1 = screenCenter.y + coneLength * std::sin(angle - coneAngle);
    float x2 = screenCenter.x + coneLength * std::cos(angle + coneAngle);
    float y2 = screenCenter.y + coneLength * std::sin(angle + coneAngle);

    al_draw_filled_triangle(screenCenter.x, screenCenter.y, x1, y1, x2, y2, al_map_rgb(170 * 0.425, 170 * 0.425, 170));

    for (auto& obj : StructureGroup->GetObjects()) {
        if (!obj || !obj->isLightSource) continue;
        Engine::Point lightCenter = obj->Position - camera;
        for (int i = 0; i < 2; ++i) {
            float r = baseRadius - i * 10.0f;
            int brightness =  160 + i * 40;
            brightness = std::min(brightness, 255);
            al_draw_filled_circle(
                lightCenter.x, lightCenter.y,
                r,
                al_map_rgb(brightness, brightness * 0.8, brightness * 0.5)
            );
        }
        
    }

    // Blend onto screen using multiplicative light blending
    al_set_target_bitmap(oldTarget);
    int op, src, dst;
    al_get_blender(&op, &src, &dst);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_SRC_COLOR);
    al_draw_bitmap(darknessOverlay, 0, 0, 0);
    al_set_blender(op, src, dst);
}

bool PlayScene::IsMouseOverUI(int mx, int my)
{
    Engine::Point mouse(mx, my);
    // Check Pause Button
    if (pauseButton && pauseButton->Visible)
    {
        Engine::Point pos = pauseButton->Position;
        Engine::Point size = pauseButton->Size;
        if (Engine::Collider::IsPointInRect(mouse, pos, size))
        {
            return true;
        }
    }

    // Check Structure Panel
    if (StructurePanel && StructurePanel->Visible)
    {
        Engine::Point pos = StructurePanel->Position;
        Engine::Point size = StructurePanel->Size;
        if (Engine::Collider::IsPointInRect(mouse, pos, size))
        {
            return true;
        }
    }
    return false;
}

bool PlayScene::IsMochiHealing() const
{
    if (activeCompanion && std::string(companionType) == "COMP2")
    {
        return activeCompanion->IsMochiHealingOngoing();
    }
    return false;
}
