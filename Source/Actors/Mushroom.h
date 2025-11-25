//
// Created by Lucas N. Ferreira on 30/09/23.
//

#pragma once

#include "Actor.h"

class Mushroom : public Actor
{
public:
    explicit Mushroom(Game* game, const std::string& uniqueName, float forwardSpeed = 100.0f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

private:
    bool mIsDying;
    float mForwardSpeed;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class TextureComponent* mDrawComponent;
};