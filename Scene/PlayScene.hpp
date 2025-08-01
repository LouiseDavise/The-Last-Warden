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
    float distFromEdge;
};

class Structure;
class Tower;
class Wall;
namespace Engine
{
    class Group;
    class Image;
    class ImageButton;
    class Panel;
    class Label;
    class Sprite;
};

class PlayScene final : public Engine::IScene
{
private:
    enum TileType
    {
        TILE_WALKABLE,
        TILE_BRIDGE,
        TILE_OCCUPIED,
        TILE_OBSTRUCTION,
        TILE_PROPERTY,
        TILE_TOXIC
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    Structure *highlightedStructure = nullptr;
    ALLEGRO_BITMAP *darknessOverlay = nullptr;
    void DrawNightTime() const;
    bool isNight = false;
    float nightCycleTimer = 0.0f;
    bool PanelVisible = true;
    bool fadingToScore = false;
    float fadeTimer = 0.0f;
    const float fadeDuration = 3.0f;

protected:
    int money = 200;
    int totalMoneyEarned = 0;
    int totalMoneySpent = 0;
    int killCount = 0;
    int waveCount = 1;
    int totalWaveSpawned = 0;
    int SpeedMult;
    Engine::Label *lifeTextLabel;

public:
    float ticks;
    float matchTime;
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static const int BlockSize;
    static Engine::Point SpawnGridPoint;
    static Engine::Point EndGridPoint;
    static const std::vector<int> CheatCode;

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
    Group *SidePanelGroup;

    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Label *UITimerLabel;
    Engine::Image *StructurePanel;
    Engine::Panel *structureInfoPanel;
    std::vector<Engine::Label *> structureInfoLabels;
    Engine::ImageButton *enhanceButton;
    Engine::Label *enhancePriceLabel;
    Engine::Image *enhanceCoinIcon;
    Engine::Image *totalCoinIcon;
    Engine::Label *totalCoin;
    Structure *preview;
    Structure *selectedStructure;

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
    void OnRightClick(int mx, int my);
    int selectedGX = -1; // last grid-x that showed the panel
    int selectedGY = -1;
    bool IsMouseOverUI(int mx, int my);

    int GetMoney() const { return money; }
    int GetTotalMoneyEarned() const { return totalMoneyEarned; }
    int GetTotalMoneySpent() const { return totalMoneySpent; }
    void AddMoney(int amount);
    bool SpendMoney(int amount);
    inline Player *GetPlayer() const
    {
        if (PlayerGroup->GetObjects().empty())
            return nullptr;
        return dynamic_cast<Player *>(PlayerGroup->GetObjects().back());
    }
    inline Player *GetCompanion() const
    {
        if (PlayerGroup->GetObjects().empty())
            return nullptr;
        return dynamic_cast<Player *>(PlayerGroup->GetObjects().front());
    }
    TileType getTileType(int value) { return static_cast<TileType>(value); }

    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);

    bool CheckSpaceValid(int x, int y);
    Structure *GetStructureAt(int gx, int gy);
    Engine::Point camera;

    // Enemy
    std::queue<EnemyWave> enemyWaves;
    void LoadEnemyWaves(const std::string &filename);
    void SpawnEnemy(const EnemyWave &wave);
    bool IsWalkable(int x, int y);
    Engine::Point lastPlayerGrid = Engine::Point(-1, -1);
    void UpdateBFSFromPlayer();
    void GenerateFlowField();
    bool validLine(Engine::Point from, Engine::Point to);
    std::string mapFile = "Resource/forest_map.txt";
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
    Engine::Image *companionProfileImage = nullptr;

    // Home and Pause Buttons
    Engine::ImageButton *homeButton = nullptr;
    Engine::ImageButton *pausePlayButton = nullptr;

    // Confirmation UI
    Engine::Image *homeConfirmBox = nullptr;
    Engine::Label *homeConfirmText1 = nullptr;
    Engine::Label *homeConfirmText2 = nullptr;
    Engine::Label *homeWarning1 = nullptr;
    Engine::Label *homeWarning2 = nullptr;
    Engine::ImageButton *homeConfirmBtn = nullptr;
    Engine::ImageButton *homeCancelBtn = nullptr;
    Engine::Label *homeConfirmLabel = nullptr;
    Engine::Label *homeCancelLabel = nullptr;

    int GetKillCount() const { return killCount; }
    int GetWaveCount() const { return waveCount; }
    void IncreaseKillCount() { killCount++; }
    void IncreaseWaveCount() { waveCount++; }
    std::shared_ptr<Companion> activeCompanion;
    bool IsMochiHealing() const;
};
#endif // PLAYSCENE_HPP
