//
// Created by ricar on 12/5/2025.
//

#pragma once
#include "DrawComponent.h"

class Texture;

class TextComponent : public DrawComponent
{
public:
    TextComponent(Actor* owner, const std::string& text, const Vector3& color, int size);

    ~TextComponent() override
    {
        if (mTexture)
            delete mTexture;
    }

    void Draw(Renderer* renderer) override;

private:
    Texture* mTexture = nullptr;
    Vector2 mSize;
    std::string mText;
    Vector3 mColor;
};
