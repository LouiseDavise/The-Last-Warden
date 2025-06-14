#include "UI/Component/Panel.hpp"
#include <allegro5/allegro_primitives.h>

namespace Engine {
    Panel::Panel(float x, float y, float w, float h, ALLEGRO_COLOR color, float anchorX, float anchorY)
        : IObject(x, y, w, h, anchorX, anchorY), Color(color) {
    }

    void Panel::Draw() const {
        al_draw_filled_rectangle(
            Position.x - Anchor.x * Size.x,
            Position.y - Anchor.y * Size.y,
            Position.x + (1 - Anchor.x) * Size.x,
            Position.y + (1 - Anchor.y) * Size.y,
            Color
        );
    }
}
