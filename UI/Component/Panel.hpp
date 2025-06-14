#ifndef PANEL_HPP
#define PANEL_HPP

#include <allegro5/color.h>
#include "Engine/IObject.hpp"

namespace Engine {
    class Panel : public IObject {
    public:
        ALLEGRO_COLOR Color;

        /// <summary>
        /// Creates a rectangular panel with the specified position, size, and color.
        /// </summary>
        Panel(float x, float y, float w, float h, ALLEGRO_COLOR color, float anchorX = 0, float anchorY = 0);

        /// <summary>
        /// Draw the panel.
        /// </summary>
        void Draw() const override;
    };
}

#endif  // PANEL_HPP
