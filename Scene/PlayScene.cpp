#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "Player/Player.hpp"
#include "Player/Caveman.hpp"
#include "Weapon/Weapon.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/SuperTankEnemy.hpp"
#include "Enemy/ExtremeTankEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Collider.hpp"
#include "PlayScene.hpp"
#include "Structure/Turret/LaserTurret.hpp"
#include "Structure/Turret/MachineGunTurret.hpp"
#include "Structure/Turret/TurretButton.hpp"
#include "Structure/Turret/Shovel.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "player_data.h"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Structure/Turret/Shovel.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
int PlayScene::MapWidth = 40, PlayScene::MapHeight = 23;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
Engine::Point PlayScene::SpawnGridPoint;
Engine::Point PlayScene::EndGridPoint;
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEY_LSHIFT, ALLEGRO_KEY_ENTER};
Engine::Point PlayScene::GetClientSize()
{
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize()
{
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    maxLives = 10;
    SpeedMult = 1;

    switch (MapId)
    {
    case 1:
        money = 150;
        break;
    case 2:
        money = 200;
        break;
    case 3:
        money = 300;
        break;
    default:
        break;
    }

    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(WeaponGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    // Read Map
    std::ifstream fin(std::string("Resource/map") + std::to_string(MapId) + ".txt");
    if (!fin.is_open())
    {
        std::cerr << "Failed to open map file\n";
        return;
    }

    std::string line;
    int maxWidth = 0;
    int height = 0;

    while (std::getline(fin, line))
    {
        if ((int)line.length() > maxWidth)
            maxWidth = line.length();
        height++;
    }

    fin.close();
    MapWidth = maxWidth;
    MapHeight = height;

    ReadMap();
    float centerX = MapWidth * BlockSize / 2.0f;
    float centerY = ((MapHeight + 3) * BlockSize / 2.0f);
    player = new Caveman(centerX, centerY);
    AddNewObject(player);
    // ReadEnemyWave();
    // mapDistance = CalculateBFSDistance();
    // ConstructUI();
    imgTarget = new Engine::Image("UI/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    // lifeTextLabel = new Engine::Label("", "pirulen.ttf", 20, 1375 + 100,  108 + 15, 255,255,255,255, 0.5f, 0.5f);
    // UIGroup->AddNewObject(lifeTextLabel);
    UIGroup->AddNewObject(imgTarget);
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
    matchTime += deltaTime;
    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
    // Calculate danger zone.
    std::vector<float> reachEndTimes;
    for (auto &it : EnemyGroup->GetObjects())
    {
        reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto &it : reachEndTimes)
    {
        if (it <= DangerTime)
        {
            danger--;
            if (danger <= 0)
            {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown)
                {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
                }
                float alpha = pos / DangerTime;
                dangerAlpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0)
    {
        AudioHelper::StopSample(deathBGMInstance);
        dangerAlpha = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;
    for (int i = 0; i < SpeedMult; i++)
    {
        IScene::Update(deltaTime);
        ticks += deltaTime;
        // Check if we should create new enemy.
        if (!enemyWaveData.empty())
        {
            auto current = enemyWaveData.front();
            if (ticks < current.second)
                continue;
            ticks -= current.second;
            enemyWaveData.pop_front();

            const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
            Enemy *enemy;
            switch (current.first)
            {
            case 1:
                EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 2:
                EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 3:
                EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            default:
                continue;
            }
            enemy->UpdatePath(mapDistance);
            // Compensate the time lost.
            enemy->Update(ticks);
        }
    }
    if (preview)
    {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        if (!preview->IsShovel())
            preview->Update(deltaTime);
    }
    auto screenSize = Engine::GameEngine::GetInstance().GetScreenSize();

    // Center camera on player
    camera.x = player->Position.x - screenSize.x / 2;
    camera.y = player->Position.y - screenSize.y / 2;

    // Clamp camera so it doesn't go outside the map
    float mapWidthInPixels = MapWidth * BlockSize;
    float mapHeightInPixels = (MapHeight + 3) * BlockSize;

    float mapWidthPx = MapWidth * BlockSize;
    float mapHeightPx = (MapHeight + 3) * BlockSize;

    camera.x = std::max(0.0f, std::min(camera.x, std::max(0.0f, mapWidthPx - screenSize.x)));
    camera.y = std::max(0.0f, std::min(camera.y, std::max(0.0f, mapHeightPx - screenSize.y)));
}
void PlayScene::Draw() const
{
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_translate_transform(&transform, -camera.x, -camera.y);
    al_use_transform(&transform);

    IScene::Draw();
    player->Draw();
    for (auto* obj : WeaponGroup->GetObjects()) {
        obj->Draw();
    }
    if (dangerAlpha > 0)
    {
        float t = al_get_time();
        float pulse = 0.5f * (1.0f + std::sin(t * 6)); // 3 Hz heartbeat
        int a = static_cast<int>(dangerAlpha * pulse);

        // full-screen red overlay
        al_draw_filled_rectangle(0, 0,
                                 PlayScene::MapWidth * BlockSize,
                                 PlayScene::MapHeight * BlockSize,
                                 al_map_rgba(255, 0, 0, a));

        // punch a transparent hole in the centre to create vignette
        al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ONE, ALLEGRO_ONE); // subtractive
        float margin = 80;                                                // width of visible rim
        al_draw_filled_rounded_rectangle(margin, margin,
                                         PlayScene::MapWidth * BlockSize - margin,
                                         PlayScene::MapHeight * BlockSize - margin,
                                         30, 30,
                                         al_map_rgba(0, 0, 0, a));         // subtract same alpha
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA); // default
    }

    // al_draw_filled_rectangle(
    //     1375, 108,
    //     1375 + 200, 108 + 30,
    //     al_map_rgb(60, 60, 60)
    // );
    // --- draw the filled portion (green) proportional to lives ---
    // float frac = float(lives) / float(maxLives);
    // al_draw_filled_rectangle(
    //     1375, 108,
    //     1375 + 200 * frac, 108 + 30,
    //     al_map_rgb(0, 200, 0)
    // );

    // // --- draw a white border around the bar ---
    // al_draw_rectangle(
    //     1375, 108,
    //     1375 + 200, 108 + 30,
    //     al_map_rgb(255, 255, 255), 3.5f
    // );

    // // --- update the text “X / Y” and draw it centered ---
    // if(lifeTextLabel){
    //     lifeTextLabel->Text = std::to_string(lives) + "/" + std::to_string(maxLives);
    //     lifeTextLabel->Draw();
    // }

    if (DebugMode)
    {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++)
        {
            for (int j = 0; j < MapWidth; j++)
            {
                if (mapDistance[i][j] != -1)
                {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my)
{
    if ((button & 1) && !imgTarget->Visible && preview)
    {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
    player->OnMouseDown(button, mx, my);
}

void PlayScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;

    if (preview && preview->IsShovel())
    {
        preview->Position = Engine::Point{(float)mx, (float)my};

        Turret *tgt = GetTurretAt(x, y);
        if (tgt != highlightedTurret)
        {
            if (highlightedTurret)
                highlightedTurret->Tint = al_map_rgba(255, 255, 255, 255);
            if (tgt)
                tgt->Tint = al_map_rgba(255, 60, 60, 255); // bright red
            highlightedTurret = tgt;
        }
        imgTarget->Visible = (tgt != nullptr);
        if (tgt)
            imgTarget->Position = Engine::Point{(float)x * BlockSize, (float)y * BlockSize};
        else
            imgTarget->Visible = false;
        return;
    }

    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my)
{
    IScene::OnMouseUp(button, mx, my);
    int gx = mx / BlockSize, gy = my / BlockSize;
    if (preview && preview->IsShovel())
    {
        if (button & 1)
        {
            Turret *t = GetTurretAt(gx, gy);
            if (t)
            {
                TowerGroup->RemoveObject(t->GetObjectIterator());
                AudioHelper::PlaySample("shovel.mp3");
                mapState[gy][gx] = TILE_FLOOR;
            }
        }
        if (highlightedTurret)
        {
            highlightedTurret->Tint = al_map_rgba(255, 255, 255, 255);
            highlightedTurret = nullptr;
        }
        return;
    }

    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1)
    {
        if (mapState[y][x] != TILE_OCCUPIED)
        {
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y))
            {
                Engine::Sprite *sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            TowerGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);
            // Remove Preview.
            preview = nullptr;

            mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);
    player->OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB)
    {
        DebugMode = !DebugMode;
    }
    else
    {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();
    }

    switch (keyCode)
    {
    case ALLEGRO_KEY_Q:
        UIBtnClicked(1);
        break;
    case ALLEGRO_KEY_W:
        UIBtnClicked(2);
        break;
    case ALLEGRO_KEY_E:
        UIBtnClicked(3);
        break;
    case ALLEGRO_KEY_SPACE:
        UIBtnClicked(0);
        break;
    default:
        break;
    }

    if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    else if (keyCode == ALLEGRO_KEY_ESCAPE && preview && preview->IsShovel())
    {
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
        return;
    }

    if (keyStrokes.size() == code.size() && std::equal(code.begin(), code.end(), keyStrokes.begin()))
    {
        EffectGroup->AddNewObject(new Plane());
        AudioHelper::PlaySample("gun.wav");
        EarnMoney(10000);
    }
}

void PlayScene::OnKeyUp(int keyCode)
{
    player->OnKeyUp(keyCode);
}

void PlayScene::Hit()
{
    lives--;
    if (UILives)
        UILives->Text = std::string("Life ");
    if (lives <= 0)
    {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}

int PlayScene::GetMoney() const { return money; }
int PlayScene::GetLives() const { return lives; }
void PlayScene::EarnMoney(int money)
{
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}

void PlayScene::ReadMap()
{
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Failed to open map file\n";
        return;
    }

    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++)
    {
        for (int j = 0; j < MapWidth; j++)
        {
            char tileChar;
            fin >> tileChar;
            switch (tileChar)
            {
            case '0':
                mapState[i][j] = TILE_DIRT;
                TileMapGroup->AddNewObject(new Engine::Image("Tileset/sand.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                break;
            case '1':
                mapState[i][j] = TILE_FLOOR;
                TileMapGroup->AddNewObject(new Engine::Image("Tileset/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                break;
            case 'S':
                mapState[i][j] = TILE_DIRT;
                TileMapGroup->AddNewObject(new Engine::Image("Tileset/sand.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                switch (MapId)
                {
                case 1:
                case 2:
                    SpawnGridPoint = Engine::Point(j - 1, i);
                    break;
                case 3:
                    SpawnGridPoint = Engine::Point(j, i);
                    break;
                }
                break;
            case 'E':
                mapState[i][j] = TILE_DIRT;
                TileMapGroup->AddNewObject(new Engine::Image("Tileset/sand.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EndGridPoint = Engine::Point(j, i);
                break;
            default:
                throw std::ios_base::failure("Unknown cell in map.");
            }
        }
    }
}

void PlayScene::ReadEnemyWave()
{
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat)
    {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}

void PlayScene::ConstructUI()
{
    auto scr = Engine::GameEngine::GetInstance().GetScreenSize();
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 20));
    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 68));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life "), "pirulen.ttf", 24, 1294, 108));

    struct BtnInfo
    {
        int x, y;
        int price;
        int btnId;
        const char *sprite;
    };
    std::vector<BtnInfo> btns = {
        {1304, 176, MachineGunTurret::Price, 1, "play/turret-1.png"},
        {1380, 176, LaserTurret::Price, 2, "play/turret-2.png"},
    };

    for (auto &b : btns)
    {
        TurretButton *btn = new TurretButton("play/floor.png", "play/dirt.png",
                                             Engine::Sprite("play/tower-base.png", b.x, b.y, 0, 0, 0, 0),
                                             Engine::Sprite(b.sprite, b.x, b.y - 8, 0, 0, 0, 0),
                                             b.x, b.y, b.price);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, b.btnId));
        UIGroup->AddNewControlObject(btn);

        auto *priceLbl = new Engine::Label(
            std::string("$") + std::to_string(b.price), "pirulen.ttf", 20, b.x + 30, b.y + 63);
        priceLbl->Anchor = Engine::Point(0.5f, 0.0f); // center‐horizontally
        UIGroup->AddNewObject(priceLbl);
    }

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;
    dangerAlpha = 0;
}

void PlayScene::UIBtnClicked(int id)
{
    if (preview)
    {
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }

    switch (id)
    {
    case 0:
        preview = new Shovel(0, 0);
        break;
    case 1:
        if (money >= MachineGunTurret::Price)
            preview = new MachineGunTurret(0, 0);
        break;
    case 2:
        if (money >= LaserTurret::Price)
            preview = new LaserTurret(0, 0);
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
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight || mapState[y][x] != TILE_FLOOR)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[SpawnGridPoint.y][SpawnGridPoint.x] == -1)
        return false;
    for (auto &it : EnemyGroup->GetObjects())
    {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0)
            pnt.x = 0;
        if (pnt.x >= MapWidth)
            pnt.x = MapWidth - 1;
        if (pnt.y < 0)
            pnt.y = 0;
        if (pnt.y >= MapHeight)
            pnt.y = MapHeight - 1;
        if (map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto &it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    return true;
}

std::vector<std::vector<int>> PlayScene::CalculateBFSDistance()
{
    std::vector<std::vector<int>> dist(MapHeight, std::vector<int>(MapWidth, -1));
    std::queue<Engine::Point> q;

    if (mapState[EndGridPoint.y][EndGridPoint.x] != TILE_DIRT)
        return dist;
    q.push(EndGridPoint);
    dist[EndGridPoint.y][EndGridPoint.x] = 0;

    while (!q.empty())
    {
        Engine::Point p = q.front();
        q.pop();
        int d = dist[p.y][p.x];

        for (auto dir : directions)
        {
            int nx = p.x + dir.x;
            int ny = p.y + dir.y;
            if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                continue;
            if (mapState[ny][nx] != TILE_DIRT)
                continue;
            if (dist[ny][nx] != -1)
                continue;

            dist[ny][nx] = d + 1;
            q.emplace(nx, ny);
        }
    }
    return dist;
}

Turret *PlayScene::GetTurretAt(int gx, int gy)
{
    for (auto *obj : TowerGroup->GetObjects())
    {
        auto *t = dynamic_cast<Turret *>(obj);
        if (!t)
            continue;
        int tx = int(t->Position.x) / BlockSize;
        int ty = int(t->Position.y) / BlockSize;
        if (tx == gx && ty == gy)
            return t;
    }
    return nullptr;
}