//
// Created by Lucas N. Ferreira on 30/09/23.
//

#pragma once

#include "Actor.h"

class Dog : public Actor
{
public:
    explicit Dog(Game* game, const std::string& uniqueName, float forwardSpeed = 100.0f, float deathTime = 0.5f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

    void DisableDamage() { mDamageEnabled = false; }
    void EnableDamage() { mDamageEnabled = true; }
    bool IsDamageEnabled() const { return mDamageEnabled; }

private:
    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;
    bool mDamageEnabled;

    class RigidBodyComponent* mRigidBodyComponent;
    AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};