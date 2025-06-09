#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Player/Player.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

struct EnemyWave
{
    int type;
    float delay;
    float count;
    bool spawned = false;
};

class Turret;
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
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
        TILE_WALL
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    Turret *highlightedTurret = nullptr;

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
    Group *BulletGroup;
    Group *TowerGroup;
    Group *EnemyGroup;
    Group *PlayerGroup;
    Group *EffectGroup;
    Group *WeaponGroup;
    Group *UIGroup;

    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Image *TargetTile;
    Engine::Image *StructurePanel;
    Turret *preview;

    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
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

    int GetMoney() const;
    inline Player* GetPlayer() const {
        if (PlayerGroup->GetObjects().empty()) return nullptr;
        return dynamic_cast<Player*>(PlayerGroup->GetObjects().front());
    }
    
    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    
    Turret *GetTurretAt(int gx, int gy);
    Engine::Point camera;

    // Enemy
    std::vector<EnemyWave> enemyWaves;
    float totalTime;
    void LoadEnemyWaves(const std::string &filename);
    void SpawnEnemy(const EnemyWave &wave);
    bool IsWalkable(int x, int y);
};
#endif // PLAYSCENE_HPP
