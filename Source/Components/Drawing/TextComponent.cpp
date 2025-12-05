//
// Created by ricar on 12/5/2025.
//

#include "TextComponent.h"
#include "../../Game.h"
#include "../../Renderer/Font.h"

TextComponent::TextComponent(Actor *owner, const std::string &text, const Vector3 &color, int size) : DrawComponent(owner, 101)
{
    // Generate a texture from text
    mTexture = owner->GetGame()
                    ->mUiFont
                    ->RenderText(text, color, size);

    mColor = color;
    mText = text;

    // Use texture size for drawing
    mSize = Vector2(mTexture->GetWidth(), mTexture->GetHeight());
}

void TextComponent::Draw(Renderer* renderer)
{
    if (!mTexture) return;

    Vector2 worldPos = mOwner->GetPosition();
    renderer->DrawTexture(
        worldPos,
        mSize,
        0.0f,
        mColor,
        mTexture,
        Vector4::UnitRect,
        mOwner->GetGame()->GetCameraPos()
    );
}
