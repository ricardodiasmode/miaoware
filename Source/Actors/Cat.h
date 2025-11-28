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

    void EatMushroomEffect();

    void EnemyHit(AABBColliderComponent *other);

private:

    void DecreaseSize();

    void ManageAnimations();
    void ReverseDirection();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;
    bool mIsBig;

    int mDirection;
    bool mAutoWalk;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AnimatorComponent* mDrawComponentBig;
    class AABBColliderComponent* mColliderComponent;
    class ParticleSystemComponent* mFireBalls;

    float mFireBallCooldown = 1.0f;
};