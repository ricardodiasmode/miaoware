//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "MovingBlock.h"
#include "../Game.h"
#include "../Components/Physics/AABBColliderComponent.h"

void MovingBlock::StartMovementInterp()
{
        if (mIsMovingInterp || !mCanMove)
                return;

        mMoveAlpha = 0.f;
        mIsMovingInterp = true;
        mInitialPosition = GetPosition();
}

MovingBlock::MovingBlock(Game* game, const std::string& uniqueName, const std::string &texturePath)
        :Block(game, uniqueName, texturePath, false)
{
}

void MovingBlock::OnUpdate(float deltaTime)
{
        Block::OnUpdate(deltaTime);

        if (mIsMovingInterp)
        {
                const float currentValue = std::sin(mMoveAlpha * M_PI)*MOVE_MULTIPLIER;

                Vector2 newPosition = mInitialPosition;
                newPosition.y -= currentValue;
                SetPosition(newPosition);

                mMoveAlpha += deltaTime*SPEED_MULTIPLIER;

                if (mMoveAlpha >= 1.f)
                {
                        mIsMovingInterp = false;
                        SetPosition(mInitialPosition);
                        OnFinishMovement();
                }
        }
}
