//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "RigidBodyComponent.h"
#include <vector>
#include <set>

enum class ColliderLayer
{
    Player,
    Enemy,
    Blocks
};

class AABBColliderComponent : public Component
{
public:

    AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                                ColliderLayer layer, bool isStatic = false, int updateOrder = 10);
    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent& b);
    bool IntersectX(const AABBColliderComponent& b);
    bool IntersectY(const AABBColliderComponent& b);

    float DetectHorizontalCollision(RigidBodyComponent *rigidBody);
    float DetectVerticalCollision(RigidBodyComponent *rigidBody);

    Vector2 GetMin() const;
    Vector2 GetMax() const;
    ColliderLayer GetLayer() const { return mLayer; }

    // Drawing for debug purposes
    void DebugDraw(class Renderer* renderer) override;

    void Resize(int w, int h, Vector2 offset);

    int mWidth;
    int mHeight;
    Vector2 mOffset;
    int mOrigWidth;
    int mOrigHeight;
    Vector2 mOrigOffset;


private:
    float GetMinVerticalOverlap(AABBColliderComponent* b);
    float GetMinHorizontalOverlap(AABBColliderComponent* b);

    void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap) const;
    void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap) const;


    bool mIsStatic;

    ColliderLayer mLayer;
};