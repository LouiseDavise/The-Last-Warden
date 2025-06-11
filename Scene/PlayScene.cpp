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
#include <limits>

#include "Player/Player.hpp"
#include "Player/Caveman.hpp"
#include "Weapon/Weapon.hpp"
#include "Weapon/SpearWeapon.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Collider.hpp"
#include "PlayScene.hpp"
#include "Structure/Offense/BowTower.hpp"
#include "Structure/Offense/LaserTurret.hpp"
#include "Structure/Offense/MachineGunTurret.hpp"
#include "Structure/StructureButton.hpp"
#include "Structure/Shovel.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "player_data.h"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/GreenSlime.hpp"
#include "Enemy/ToxicSlime.hpp"
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
    money = 10000;

    AddNewObject(TileMapGroup = new Group("TileMapGroup"));
    AddNewObject(GroundEffectGroup = new Group("GroundEffectGroup"));
    AddNewObject(DebugIndicatorGroup = new Group("DebugGroup"));
    AddNewObject(ProjectileGroup = new Group("ProjectileGroup"));
    AddNewObject(TowerGroup = new Group("TowerGroup"));
    AddNewObject(EnemyGroup = new Group("EnemyGroup"));
    AddNewObject(PlayerGroup = new Group("PlayerGroup"));
    AddNewObject(WeaponGroup = new Group("WeaponGroup"));
    AddNewObject(EffectGroup = new Group("EffectGroup"));
    AddNewControlObject(UIGroup = new Group("UIGroup"));

    std::ifstream fin("Resource/map1.txt");
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
    ReadMap();
    float centerX = MapWidth * BlockSize / 2.0f;
    float centerY = ((MapHeight + 3) * BlockSize / 2.0f);

    if (isTwoPlayer)
    {
        companion = new Companion(centerX + 64, centerY);
        PlayerGroup->AddNewObject(companion);
    }

    Player *player = new Caveman(centerX, centerY);
    PlayerGroup->AddNewObject(player);

    // Calculate distances from player
    // Use BFS to fill mapDistance (already in your code, assumed)
    UpdateBFSFromPlayer(); // <- You might already have this.
    // Then:
    GenerateFlowField();
    LoadEnemyWaves(waveFile);

    int w = al_get_display_width(al_get_current_display());
    int h = al_get_display_height(al_get_current_display());

    preview = nullptr;
    TargetTile = new Engine::Image("UI/target.png", 0, 0);
    TargetTile->Visible = false;
    UIGroup->AddNewObject(TargetTile);

    StructurePanel = new Engine::Image("UI/Structure_panel.png", w / 2 - 332, h - 88, 664, 88, 0, 0);
    StructurePanel->Visible = true;
    UIGroup->AddNewObject(StructurePanel);

    // lifeTextLabel = new Engine::Label("", "pirulen.ttf", 20, 1375 + 100,  108 + 15, 255,255,255,255, 0.5f, 0.5f);
    // UIGroup->AddNewObject(lifeTextLabel);
    ConstructUI();
    UITimerLabel = new Engine::Label("00:00:00", "pirulen.ttf", 30, al_get_display_width(al_get_current_display()) / 2, 20, 255, 255, 255, 255, 0.5f, 0.0f);
    UIGroup->AddNewObject(UITimerLabel);

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

            if (player->GetHP() > 0 && enemy->GetHP() > 0)
                player->TakeDamage(enemy->GetDamage(), enemy->Position);
        }
        IScene::Update(deltaTime);
        ticks += deltaTime;
        if (player && player->GetHP() <= 0)
        {
            Engine::GameEngine::GetInstance().ChangeScene("lose-scene");
            return; // stop further updates this frame
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
}

void PlayScene::Draw() const
{
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_translate_transform(&transform, -camera.x, -camera.y);
    al_use_transform(&transform);
    TileMapGroup->Draw();
    GroundEffectGroup->Draw();
    EnemyGroup->Draw();
    PlayerGroup->Draw();
    WeaponGroup->Draw();
    ProjectileGroup->Draw();
    EffectGroup->Draw();
    TowerGroup->Draw();

    al_identity_transform(&transform);
    al_use_transform(&transform);
    UIGroup->Draw();
}

void PlayScene::OnMouseDown(int button, int mx, int my)
{
    Player *player = GetPlayer();
    if ((button & 1) && !TargetTile->Visible && preview)
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
    int x = (mx + camera.x) / BlockSize;
    int y = (my + camera.y) / BlockSize;

    if (preview && preview->IsShovel())
    {
        preview->Position = Engine::Point{(float)mx, (float)my};

        Tower *tgt = GetTowerAt(x, y);
        if (tgt != highlightedTower)
        {
            if (highlightedTower)
                highlightedTower->Tint = al_map_rgba(255, 255, 255, 255);
            if (tgt)
                tgt->Tint = al_map_rgba(255, 60, 60, 255); // bright red
            highlightedTower = tgt;
        }
        TargetTile->Visible = (tgt != nullptr);
        if (tgt)
            TargetTile->Position = Engine::Point{(float)x * BlockSize, (float)y * BlockSize};
        else
            TargetTile->Visible = false;
        return;
    }

    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        TargetTile->Visible = false;
        return;
    }
    TargetTile->Visible = true;
    TargetTile->Position.x = x * BlockSize;
    TargetTile->Position.y = y * BlockSize;
}

void PlayScene::OnMouseUp(int button, int mx, int my)
{
    IScene::OnMouseUp(button, mx, my);
    int gx = (mx + camera.x) / BlockSize;
    int gy = (my + camera.y) / BlockSize;
    if (preview && preview->IsShovel())
    {
        if (button & 1)
        {
            Tower *t = GetTowerAt(gx, gy);
            if (t)
            {
                TowerGroup->RemoveObject(t->GetObjectIterator());
                AudioHelper::PlaySample("shovel.mp3");
                mapState[gy][gx] = TILE_FLOOR;
            }
        }
        if (highlightedTower)
        {
            highlightedTower->Tint = al_map_rgba(255, 255, 255, 255);
            highlightedTower = nullptr;
        }
        return;
    }

    if (!TargetTile->Visible)
        return;
    int x = (mx + camera.x) / BlockSize;
    int y = (my + camera.y) / BlockSize;
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
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("UI/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
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
            keyStrokes.pop_front();
    }
    if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    else if (keyCode == ALLEGRO_KEY_ESCAPE && preview && preview->IsShovel())
    {
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
        return;
    }

    if (keyStrokes.size() == CheatCode.size() && std::equal(CheatCode.begin(), CheatCode.end(), keyStrokes.begin()))
    {
        EffectGroup->AddNewObject(new Plane());
        AudioHelper::PlaySample("gun.wav");
        EarnMoney(10000);
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

int PlayScene::GetMoney() const { return money; }

void PlayScene::EarnMoney(int money)
{
    this->money += money;
}

void PlayScene::SetMapFile(const std::string &filename)
{
    mapFile = filename;
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
            case '4':
            {
                mapState[i][j] = TILE_FLOOR;

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
    return mapState[y][x] != TILE_WALL;
}

void PlayScene::ReadEnemyWave()
{
    std::string filename = std::string("Resource/enemy.txt");
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

        Enemy* toSpawn;
        switch(wave.type){
            case 0 : toSpawn = new GreenSlime(spawnX, spawnY); break;
            case 1 : toSpawn = new ToxicSlime(spawnX, spawnY); break;
        }
        if (!mapDistance.empty() && mapDistance[gy][gx] != -1)
        {
            toSpawn->UpdatePath(mapDistance);
        }

        EnemyGroup->AddNewObject(toSpawn);
    }
}

void PlayScene::ConstructUI()
{
    int w = al_get_display_width(al_get_current_display());
    int h = al_get_display_height(al_get_current_display());
    struct BtnInfo
    {
        int x, y;
        int price;
        int btnId;
        const char *sprite;
    };
    std::vector<BtnInfo> btns = {
        {w / 2 - 332 + 6 + 74 * 0, h - 82, BowTower::Price, 1, "Structures/BowTower.png"},
        {w / 2 - 332 + 6 + 74 * 1, h - 82, MachineGunTurret::Price, 2, "Structures/turret-1.png"},
        {w / 2 - 332 + 6 + 74 * 2, h - 82, LaserTurret::Price, 3, "Structures/turret-2.png"}};

    for (auto &b : btns)
    {
        StructureButton *btn = new StructureButton("UI/structurebtn.png", "UI/structurebtn_hovered.png",
                                                   Engine::Sprite("Structures/tower-base.png", b.x + 37, b.y + 38, 0, 0, 0.5, 0.5),
                                                   Engine::Sprite(b.sprite, b.x + 37, b.y + 30, 0, 0, 0.5, 0.5),
                                                   b.x, b.y, b.price);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, b.btnId));
        UIGroup->AddNewControlObject(btn);

        auto *priceLbl = new Engine::Label(
            std::string("$") + std::to_string(b.price), "pirulen.ttf", 17, b.x + 34, b.y + 62);
        priceLbl->Anchor = Engine::Point(0.5f, 0.0f); // center‐horizontally
        UIGroup->AddNewObject(priceLbl);
    }
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
        if (money >= BowTower::Price)
            preview = new BowTower(0, 0);
        break;
    case 2:
        if (money >= MachineGunTurret::Price)
            preview = new MachineGunTurret(0, 0);
        break;
    case 3:
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
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;

    if (mapState[y][x] == TILE_OCCUPIED || mapState[y][x] == TILE_WALL)
        return false;

    // Only allow placement on TILE_DIRT or TILE_FLOOR
    if (mapState[y][x] != TILE_DIRT && mapState[y][x] != TILE_FLOOR)
        return false;

    return true;
}

Tower *PlayScene::GetTowerAt(int gx, int gy)
{
    for (auto *obj : TowerGroup->GetObjects())
    {
        auto *t = dynamic_cast<Tower *>(obj);
        if (!t)
            continue;
        int tx = int(t->Position.x) / BlockSize;
        int ty = int(t->Position.y) / BlockSize;
        if (tx == gx && ty == gy)
            return t;
    }
    return nullptr;
}