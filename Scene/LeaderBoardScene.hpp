#ifndef LEADERBOARDSCENE_HPP
#define LEADERBOARDSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include <string>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

struct LeaderboardEntry
{
    std::string uid;
    std::string name;
    int score;
    std::string time;
    std::string date;
};

class LeaderBoardScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<LeaderboardEntry> leaderboard;
    std::vector<Engine::Label*> pageLabels;
    int currentPage;
    const int entriesPerPage = 5;

    void LoadLeaderboard();
    void SortLeaderboard();

public:
    explicit LeaderBoardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void ScoreboardOnClick();
    void BackOnClick(int stage);
    void OnNextClick();
    void OnBackClick();
    void RenderPage();
    void ClearPageLabels();
};

#endif // LEADERBOARDSCENE_HPP
