//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Block.h"

class MovingBlock : public Block
{
    static constexpr float MOVE_MULTIPLIER = 20.f;
    static constexpr float SPEED_MULTIPLIER = 4.f;

private:
    bool mIsMovingInterp = false;
    float mMoveAlpha = 0.f;
    Vector2 mInitialPosition;

protected:
    bool mCanMove = true;

protected:
    virtual void OnFinishMovement() {}

public:
    explicit MovingBlock(Game* game, const std::string &texturePath);

    void OnUpdate(float deltaTime) override;

    void StartMovementInterp();
};
