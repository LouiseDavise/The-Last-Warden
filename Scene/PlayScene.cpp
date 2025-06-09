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
#include <random>

#include "Player/Player.hpp"
#include "Player/Caveman.hpp"
#include "Weapon/Weapon.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Enemy/Enemy.hpp"
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
#include "Enemy/SlimeEnemy.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
int PlayScene::MapWidth = 40, PlayScene::MapHeight = 23;
const int PlayScene::BlockSize = 64;
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
    SpeedMult = 1;
    totalTime = 0;
    matchTime = 0;

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
    AddNewObject(TileMapGroup = new Group("TileMapGroup"));
    AddNewObject(GroundEffectGroup = new Group("GroundEffectGroup"));
    AddNewObject(DebugIndicatorGroup = new Group("DebugGroup"));
    AddNewObject(TowerGroup = new Group("TowerGroup"));
    AddNewObject(EnemyGroup = new Group("EnemyGroup"));
    AddNewObject(PlayerGroup = new Group("PlayerGroup"));
    AddNewObject(WeaponGroup = new Group("WeaponGroup"));
    AddNewObject(BulletGroup = new Group("BulletGroup"));
    AddNewObject(EffectGroup = new Group("EffectGroup"));
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group("UIGroup"));
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

    PlayerGroup->AddNewObject(new Caveman(centerX, centerY));
    LoadEnemyWaves("Resource/enemy1.txt");
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
    Player *player = GetPlayer();
    matchTime += deltaTime;
    for (auto &wave : enemyWaves)
    {
        if (!wave.spawned && matchTime >= wave.delay)
        {
            SpawnEnemy(wave);
            wave.spawned = true;
        }
    }

    for (int i = 0; i < SpeedMult; i++)
    {
        for (auto *obj : EnemyGroup->GetObjects())
        {
            Enemy *enemy = dynamic_cast<Enemy *>(obj);
            if (!enemy)
                continue;

            if (!Engine::Collider::IsCircleOverlap(player->Position, player->GetRadius(), enemy->Position, enemy->GetRadius()))
                continue;

            if (player->GetHP() > 0)
                player->TakeDamage(enemy->GetDamage(), enemy->Position);
        }
        IScene::Update(deltaTime);
        ticks += deltaTime;
        if (player && player->IsDead())
        {
            Engine::GameEngine::GetInstance().ChangeScene("lose-scene");
            return; // stop further updates this frame
        }
        // Enemy Collision
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
    for (auto *obj : WeaponGroup->GetObjects())
    {
        obj->Draw();
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
    Player *player = GetPlayer();
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
    // if (button & 1)
    // {
    //     if (mapState[y][x] != TILE_OCCUPIED)
    //     {
    //         if (!preview)
    //             return;
    //         // Check if valid.
    //         if (!CheckSpaceValid(x, y))
    //         {
    //             Engine::Sprite *sprite;
    //             GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
    //             sprite->Rotation = 0;
    //             return;
    //         }
    //         // Purchase.
    //         EarnMoney(-preview->GetPrice());
    //         // Remove Preview.
    //         preview->GetObjectIterator()->first = false;
    //         UIGroup->RemoveObject(preview->GetObjectIterator());
    //         // Construct real turret.
    //         preview->Position.x = x * BlockSize + BlockSize / 2;
    //         preview->Position.y = y * BlockSize + BlockSize / 2;
    //         preview->Enabled = true;
    //         preview->Preview = false;
    //         preview->Tint = al_map_rgba(255, 255, 255, 255);
    //         TowerGroup->AddNewObject(preview);
    //         // To keep responding when paused.
    //         preview->Update(0);
    //         // Remove Preview.
    //         preview = nullptr;

    //         mapState[y][x] = TILE_OCCUPIED;
    //         OnMouseMove(mx, my);
    //     }
    // }
}
void PlayScene::OnKeyDown(int keyCode)
{
    Player *player = GetPlayer();
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
    Player *player = GetPlayer();
    player->OnKeyUp(keyCode);
}

int PlayScene::GetMoney() const { return money; }

void PlayScene::EarnMoney(int money)
{
    this->money += money;
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

    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
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
                mapState[i][j] = TILE_DIRT;
                break;

                // case '1':
                // {
                //     mapState[i][j] = TILE_DIRT;
                //     int treeIdx = treeDist(rng);
                //     std::string treePath = "Tileset/tree/image1x" + std::to_string(treeIdx) + ".png";
                //     TileMapGroup->AddNewObject(new Engine::Image(treePath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     break;
                // }

            case '1':
            {
                mapState[i][j] = TILE_WALL;

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

                // 4-way cross
                if (up && down && left && right)
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
                mapState[i][j] = TILE_WALL;

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
                mapState[i][j] = TILE_WALL;

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

            case 'S':
                mapState[i][j] = TILE_DIRT;
                SpawnGridPoint = (MapId == 3) ? Engine::Point(j, i) : Engine::Point(j - 1, i);
                break;

            case 'E':
                mapState[i][j] = TILE_DIRT;
                EndGridPoint = Engine::Point(j, i);
                break;

            default:
                mapState[i][j] = TILE_DIRT;
                break;
            }
            if (tileChar == '0' && prob(rng) < treeChance)
            {
                int treeIdx = treeDist(rng);
                std::string treePath = "Tileset/tree/image1x" + std::to_string(treeIdx) + ".png";
                TileMapGroup->AddNewObject(new Engine::Image(treePath, j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
        }
    }
}

bool PlayScene::IsWalkable(int x, int y)
{
    return mapState[y][x] != TILE_WALL;
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

void PlayScene::LoadEnemyWaves(const std::string &filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Failed to open enemy file\n";
        return;
    }

    int type;
    float delay, count;
    while (fin >> type >> delay >> count)
    {
        enemyWaves.push_back({type, delay, count, false});
    }
}

void PlayScene::SpawnEnemy(const EnemyWave &wave)
{
    if (wave.type == 0)
    {
        Player *player = GetPlayer();
        Engine::Point playerPos = player->Position;

        static std::default_random_engine rng((std::random_device())());
        std::uniform_real_distribution<float> angleDist(0, 2 * 3.1415926f);
        float radius = 500.0f;

        for (int i = 0; i < wave.count; ++i)
        {
            float angle = angleDist(rng);
            float spawnX = playerPos.x + std::cos(angle) * radius;
            float spawnY = playerPos.y + std::sin(angle) * radius;

            // Convert to grid coordinates
            int gx = static_cast<int>(spawnX) / BlockSize;
            int gy = static_cast<int>(spawnY) / BlockSize;

            // Check if inside map and valid tile
            if (gx < 0 || gx >= MapWidth || gy < 0 || gy >= MapHeight)
                continue;
            if (mapState[gy][gx] != TILE_DIRT)
                continue;

            SlimeEnemy *slime = new SlimeEnemy(spawnX, spawnY);
            if (!mapDistance.empty() && mapDistance[gy][gx] != -1)
            {
                slime->UpdatePath(mapDistance);
            }

            EnemyGroup->AddNewObject(slime);
        }
    }
}

void PlayScene::ConstructUI()
{
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