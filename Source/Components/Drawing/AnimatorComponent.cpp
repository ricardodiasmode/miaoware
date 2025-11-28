//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include <fstream>

AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, const float xOffset, const float yOffset,  int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
        ,mXOffset(xOffset)
        ,mYOffset(yOffset)
{
    mSpriteTexture = owner->GetGame()->GetRenderer()->GetTexture(texPath);
    assert(mSpriteTexture != nullptr);

    if (!dataPath.empty())
        LoadSpriteSheetData(dataPath);
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x)/textureWidth, static_cast<float>(y)/textureHeight,
                                      static_cast<float>(w)/textureWidth, static_cast<float>(h)/textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer)
{
    if (!mIsVisible)
        return;

    Vector2 position = mOwner->GetPosition();
    position.x += mXOffset;
    position.y += mYOffset;
    float rotation = mOwner->GetRotation();

    Vector2 size(static_cast<float>(mWidth), static_cast<float>(mHeight));
    size.x *= GetOwner()->GetScale().x;
    size.y *= GetOwner()->GetScale().y;

    Vector3 color(1.0f, 1.0f, 1.0f);

    Vector4 texRect(0.0f, 0.0f, 1.0f, 1.0f);

    if (mAnimations.count(mAnimName) > 0)
    {
        const int currentFrame = static_cast<int>(mAnimTimer);
        const int animIndex = mAnimations[mAnimName][currentFrame];
        texRect = mSpriteSheetData[animIndex];
    }

    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();

    bool flip = false;

    float textureFactor = mTextureFactor;

    renderer->DrawTexture(position, size, rotation, color, mSpriteTexture, texRect, cameraPos, flip, textureFactor);
}

void AnimatorComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.empty() || mAnimations.count(mAnimName) == 0)
    {
        return;
    }

    mAnimTimer += mAnimFPS * deltaTime;

    if (mAnimTimer >= mAnimations[mAnimName].size())
        mAnimTimer = 0.0f;
}

void AnimatorComponent::SetAnimation(const std::string& name)
{
    mAnimName = name;
    Update(0.0f);
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}