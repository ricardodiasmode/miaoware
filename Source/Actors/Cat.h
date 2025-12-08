//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

class Cat : public Actor
{
public:
    explicit Cat(Game* game, const std::string& uniqueName, float forwardSpeed = 180.0f, float jumpSpeed = -750.0f);

    void Jump();

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void OnVerticalCollision(float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

    void EnemyHit(AABBColliderComponent *other);

    bool IsCatDead() {return mIsDead;};

private:

    void DecreaseSize();

    void ManageAnimations();
    void ReverseDirection();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;
    bool mWalkingSfxPlaying = false;

    int mDirection;
    int mCanJump = true;
    bool mAutoWalk;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};