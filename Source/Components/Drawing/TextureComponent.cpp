//
// TextureComponent.cpp
//

#include "TextureComponent.h"

#include <cassert>

#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Texture.h"
#include "../../Renderer/Renderer.h"

TextureComponent::TextureComponent(class Actor* owner, const std::string& texPath,
                                   int width, int height, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mWidth(width)
    , mHeight(height)
    , mTextureFactor(1.0f)
{
    mTexture = owner->GetGame()->GetRenderer()->GetTexture(texPath);
    assert(mTexture != nullptr);
}

void TextureComponent::Draw(Renderer* renderer)
{
    if (!mIsVisible)
        return;

    Vector2 position = mOwner->GetPosition();
    float rotation = mOwner->GetRotation();
    Vector2 size(static_cast<float>(mWidth), static_cast<float>(mHeight));
    size.x *= GetOwner()->GetScale().x;
    size.y *= GetOwner()->GetScale().y;
    Vector3 color(1.0f, 1.0f, 1.0f);
    Vector4 texRect(0.0f, 0.0f, 1.0f, 1.0f);

    renderer->DrawTexture(position, size, rotation, color, mTexture, texRect, mOwner->GetGame()->GetCameraPos());
}