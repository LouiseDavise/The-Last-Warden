#include "UI/Component/Panel.hpp"
#include <allegro5/allegro_primitives.h>
#include "Engine/Resources.hpp"

namespace Engine {
    Panel::Panel(float x, float y, float w, float h, ALLEGRO_COLOR color,
                 float anchorX, float anchorY, const std::string& titleText)
        : IObject(x, y, w, h, anchorX, anchorY), Color(color) {
        
        float px = Position.x - Anchor.x * Size.x;
        float py = Position.y - Anchor.y * Size.y;

        // Create title label (font size 24)
        TitleLabel = new Label(titleText, "pirulen.ttf", 24, px + 20, py + 10, 255, 255, 255);
        
        // Create 6 labels (font size 18), spaced vertically
        for (int i = 0; i < 6; ++i) {
            float ly = py + 50 + i * 30;
            Label* lbl = new Label("", "pirulen.ttf", 18, px + 20, ly, 255, 255, 255);
            InfoLabels.push_back(lbl);
        }
    }

    Panel::~Panel() {
        delete TitleLabel;
        for (auto* lbl : InfoLabels) delete lbl;
    }

    void Panel::SetTitle(const std::string& titleText) {
        TitleLabel->Text = titleText;
    }

    void Panel::SetLabel(size_t index, const std::string& text) {
        if (index < InfoLabels.size()) {
            InfoLabels[index]->Text = text;
        }
    }

    void Panel::Draw() const {
        // Draw panel background
        al_draw_filled_rectangle(
            Position.x - Anchor.x * Size.x,
            Position.y - Anchor.y * Size.y,
            Position.x + (1 - Anchor.x) * Size.x,
            Position.y + (1 - Anchor.y) * Size.y,
            Color
        );
        if (Visible) {
            TitleLabel->Draw();
            for (auto* lbl : InfoLabels) {
                if (lbl->Visible) lbl->Draw();
            }
        }
    }
}
