#include <functional>
#include <memory>

#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Image.hpp"
#include "ImageButton.hpp"

namespace Engine
{
    ImageButton::ImageButton(std::string img, std::string imgIn, float x, float y, float w, float h, float anchorX, float anchorY) : Image(img, x, y, w, h, anchorX, anchorY), imgOut(Resources::GetInstance().GetBitmap(img)), imgIn(Resources::GetInstance().GetBitmap(imgIn))
    {
        Point mouse = GameEngine::GetInstance().GetMousePosition();
        mouseIn = Collider::IsPointInBitmap(
            Point((mouse.x - Position.x) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(),
                  (mouse.y - Position.y) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()),
            imgOut);
        if (!mouseIn || !Enabled)
            bmp = imgOut;
        else
            bmp = this->imgIn;
        imgOverlay = Resources::GetInstance().GetBitmap("UI/button-transparant.png");
    }
    void ImageButton::SetOnClickCallback(std::function<void(void)> onClickCallback)
    {
        OnClickCallback = onClickCallback;
    }
    void ImageButton::OnMouseDown(int button, int mx, int my)
    {
        if ((button & 1) && mouseIn && Enabled)
        {
            if (OnClickCallback)
                OnClickCallback();
        }
    }
    void ImageButton::OnMouseMove(int mx, int my)
    {
        mouseIn = Collider::IsPointInBitmap(
            Point((mx - Position.x) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(),
                  (my - Position.y) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()),
            imgOut);
        if (!mouseIn || !Enabled)
            bmp = imgOut;
        else
            bmp = imgIn;
    }

    void ImageButton::Draw() const
    {
        ALLEGRO_BITMAP *rawBmp = bmp.get();
        bool isButtonImage = (imgOut.get() == Resources::GetInstance().GetBitmap("UI/button.png").get());

        if (mouseIn && Enabled)
        {
            if (isButtonImage && imgOverlay)
            {
                // Apply opacity to imgIn.
                al_draw_tinted_scaled_bitmap(
                    imgIn.get(),
                    al_map_rgba_f(1.0, 1.0, 1.0, HoverOpacity), // Tint with alpha
                    0, 0,
                    al_get_bitmap_width(imgIn.get()), al_get_bitmap_height(imgIn.get()),
                    Position.x - Anchor.x * Size.x,
                    Position.y - Anchor.y * Size.y,
                    Size.x, Size.y,
                    0);
                al_draw_tinted_scaled_bitmap(
                    imgOverlay.get(),
                    al_map_rgba_f(1.0, 1.0, 1.0, 1.0), // fully opaque overlay (adjust if needed)
                    0, 0,
                    al_get_bitmap_width(imgOverlay.get()), al_get_bitmap_height(imgOverlay.get()),
                    Position.x - Anchor.x * Size.x,
                    Position.y - Anchor.y * Size.y,
                    Size.x, Size.y,
                    0);
            }
            else
            {
                al_draw_scaled_bitmap(
                    rawBmp,
                    0, 0,
                    al_get_bitmap_width(rawBmp), al_get_bitmap_height(rawBmp),
                    Position.x - Anchor.x * Size.x,
                    Position.y - Anchor.y * Size.y,
                    Size.x, Size.y,
                    0);
            }
        }
        else
        {
            // Normal draw
            al_draw_scaled_bitmap(
                rawBmp,
                0, 0,
                al_get_bitmap_width(rawBmp), al_get_bitmap_height(rawBmp),
                Position.x - Anchor.x * Size.x,
                Position.y - Anchor.y * Size.y,
                Size.x, Size.y,
                0);
        }
    }

}
