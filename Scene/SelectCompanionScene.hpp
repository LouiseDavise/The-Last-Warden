#ifndef SELECT_COMPANION_SCENE_HPP
#define SELECT_COMPANION_SCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

class SelectCompanionScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int pendingCompanionId = -1;
    Engine::Image *confirmBox = nullptr;
    Engine::Label *confirmText = nullptr;
    Engine::Label *warningText1 = nullptr;
    Engine::Label *warningText2 = nullptr;
    Engine::ImageButton *confirmBtn = nullptr;
    Engine::ImageButton *cancelBtn = nullptr;
    Engine::Label *confirmLabel = nullptr;
    Engine::Label *cancelLabel = nullptr;

public:
    explicit SelectCompanionScene() = default;
    void Initialize() override;
    void Terminate() override;
    void OnSelectClick(int compId);  // 0 = COMP1, 1 = COMP2, 2 = COMP3
};

#endif
