#ifndef WELCOMESCENE_HPP
#define WELCOMESCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include <string>
#include <vector>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

class WelcomeScene final : public Engine::IScene
{
private:
    struct LeaderboardEntry
    {
        std::string uid;
        std::string name;
        int score;
        std::string time;
        std::string date;
    };

    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<LeaderboardEntry> filteredEntries;
    std::shared_ptr<class ImageButton> continueButton;

    void LoadPlayerHistory(const std::string &path, const std::string &uid);

public:
    int currentPage = 0;
    const int entriesPerPage = 8;
    std::vector<Engine::Label *> pageLabels;

    void RenderPage();
    void ClearPageLabels();
    void OnNextClick();
    void OnBackClick();

    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void OnContinueClicked();
};

#endif // WELCOMESCENE_HPP
