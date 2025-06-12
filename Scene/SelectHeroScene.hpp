#ifndef SELECT_HERO_SCENE_HPP
#define SELECT_HERO_SCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

class SelectHeroScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int pendingHeroId = -1;
    Engine::Image *confirmBox = nullptr;
    Engine::Label *confirmText = nullptr;
    Engine::Label *warningText1 = nullptr;
    Engine::Label *warningText2 = nullptr;
    Engine::ImageButton *confirmBtn = nullptr;
    Engine::ImageButton *cancelBtn = nullptr;
    Engine::Label *confirmLabel = nullptr;
    Engine::Label *cancelLabel = nullptr;

public:
    explicit SelectHeroScene() = default;
    void Initialize() override;
    void Terminate() override;
    void OnSelectClick(int heroId); // 0 = Spearman, 1 = Archer, 2 = Mage
};

#endif // SELECT_HERO_SCENE_HPP
