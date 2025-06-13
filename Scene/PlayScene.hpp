#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <queue>

#include "Player/Player.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Player/Companion.hpp"

struct EnemyWave
{
    float timestamp;
    int type;
    float count;
};

class Structure;
class Tower;
class Wall;
namespace Engine
{
    class Group;
    class Image;
    class ImageButton;
    class Label;
    class Sprite;
}

class PlayScene final : public Engine::IScene
{
private:
    enum TileType
    {
        TILE_WALKABLE,
        TILE_BRIDGE,
        TILE_OCCUPIED,
        TILE_OBSTRUCTION,
        TILE_PROPERTY
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    Structure *highlightedStructure = nullptr;
    ALLEGRO_BITMAP* darknessOverlay = nullptr;
    void DrawNightTime() const;
    bool isNight = false;
    float nightCycleTimer = 0.0f;
    bool PanelVisible = true;

protected:
    int money;
    int SpeedMult;
    Engine::Label *lifeTextLabel;

public:
    float matchTime;
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static const int BlockSize;
    static Engine::Point SpawnGridPoint;
    static Engine::Point EndGridPoint;
    static const std::vector<int> CheatCode;
    float ticks;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *ProjectileGroup;
    Group *StructureGroup;
    Group *EnemyGroup;
    Group *PlayerGroup;
    Group *EffectGroup;
    Group *WeaponGroup;
    Group *UIGroup;
    Group *PanelGroup;

    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Label *UITimerLabel;
    Engine::Image *TargetTile;
    Engine::Image *StructurePanel;
    Engine::Image *totalCoinIcon;
    Engine::Label *totalCoin;
    Structure *preview;

    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::vector<std::vector<Engine::Point>> flowField;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    bool IsMouseOverUI(int mx, int my);
    
    int GetMoney() const;
    inline Player *GetPlayer() const
    {
        if (PlayerGroup->GetObjects().empty())
            return nullptr;
        return dynamic_cast<Player *>(PlayerGroup->GetObjects().back());
    }
    TileType getTileType(int value) {return static_cast<TileType>(value);}

    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);

    bool CheckSpaceValid(int x, int y);
    Structure *GetStructureAt(int gx, int gy);
    Engine::Point camera;

    // Enemy
    std::queue<EnemyWave> enemyWaves;
    float totalTime;
    void LoadEnemyWaves(const std::string &filename);
    void SpawnEnemy(const EnemyWave &wave);
    bool IsWalkable(int x, int y);
    Engine::Point lastPlayerGrid = Engine::Point(-1, -1);
    void UpdateBFSFromPlayer();
    void GenerateFlowField();
    bool validLine(Engine::Point from, Engine::Point to);
    std::string mapFile = "Resource/map1.txt";
    void SetMapFile(const std::string &filename);
    std::string waveFile = "Resource/single_wave.txt";
    void SetWaveFile(const std::string &filename);

    bool isTwoPlayer = false;
    void SetTwoPlayerMode(bool twoP);
    Companion *companion = nullptr;
    Engine::Image *pauseButton = nullptr;
    Engine::Image *playButton = nullptr;
    bool paused = false;
    Engine::Image *playerProfileImage = nullptr;
};
#endif // PLAYSCENE_HPP
