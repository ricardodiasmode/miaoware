//
// TextureComponent.h
//

#pragma once


#include "DrawComponent.h"
#include <string>

class TextureComponent : public DrawComponent
{
public:
    TextureComponent(class Actor* owner, const std::string& texPath,
                     int width, int height, int drawOrder = 100);

    void Draw(class Renderer* renderer) override;

    void SetTextureFactor(float factor) { mTextureFactor = factor; }
    float GetTextureFactor() const { return mTextureFactor; }

private:
    class Texture* mTexture;
    int mWidth;
    int mHeight;
    float mTextureFactor;
};
