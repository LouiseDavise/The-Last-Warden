#pragma once
#include <allegro5/allegro_color.h>
#include <string>
#include <vector>
#include "Engine/IObject.hpp"
#include "Label.hpp"

namespace Engine {
    class Panel : public IObject {
    public:
        ALLEGRO_COLOR Color;
        Label* TitleLabel;
        std::vector<Label*> InfoLabels;

        Panel(float x, float y, float w, float h, ALLEGRO_COLOR color,
              float anchorX = 0.0f, float anchorY = 0.0f, const std::string& titleText = "");

        ~Panel();

        void SetTitle(const std::string& titleText);
        void SetLabel(size_t index, const std::string& text);
        void Draw() const override;
    };
}
