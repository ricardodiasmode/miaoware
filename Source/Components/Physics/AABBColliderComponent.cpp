//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Actors/Mario.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    const float minX = GetOwner()->GetPosition().x + mOffset.x;
    const float minY = GetOwner()->GetPosition().y + mOffset.y;
    const Vector2 min(minX, minY);
    return min;
}

Vector2 AABBColliderComponent::GetMax() const
{
    const float maxX = GetOwner()->GetPosition().x + mOffset.x + static_cast<float>(mWidth);
    const float maxY = GetOwner()->GetPosition().y + mOffset.y + static_cast<float>(mHeight);
    const Vector2 max(maxX, maxY);
    return max;
}

bool AABBColliderComponent::IntersectX(const AABBColliderComponent &b)
{
    return !(GetMax().x <= b.GetMin().x || b.GetMax().x <= GetMin().x);
}

bool AABBColliderComponent::IntersectY(const AABBColliderComponent &b)
{
    return !(GetMax().y <= b.GetMin().y || b.GetMax().y <= GetMin().y);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b)
{
    if (!IntersectX(b))
        return false;

    if (!IntersectY(b))
        return false;

    return true;
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b)
{
    const float overlapLeft = GetMax().x - b->GetMin().x;
    const float overlapRight = b->GetMax().x - GetMin().x;

    if (std::abs(overlapLeft) < std::abs(overlapRight))
        return -overlapLeft;
    else
        return overlapRight;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b)
{
    const float overlapTop = GetMax().y - b->GetMin().y;
    const float overlapBottom = b->GetMax().y - GetMin().y;

    if (std::abs(overlapTop) < std::abs(overlapBottom))
        return -overlapTop;
    else
        return overlapBottom;
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody)
{
    if (mIsStatic || !mIsEnabled) return 0.0f;

    float totalDisplacement = 0.0f;

    auto colliders = GetGame()->GetColliders();

    for (auto* other : colliders)
    {
        if (other == this || !other->mIsEnabled)
            continue;

        if (Intersect(*other))
        {
            const float overlapX = GetMinHorizontalOverlap(other);

            if (overlapX != 0.0f)
            {
                GetOwner()->OnHorizontalCollision(overlapX, other);

                ResolveHorizontalCollisions(rigidBody, overlapX);
                totalDisplacement += overlapX;
            }
        }
    }

    return totalDisplacement;
}

float AABBColliderComponent::DetectVerticalCollision(RigidBodyComponent *rigidBody)
{
    if (mIsStatic || !mIsEnabled) return 0.0f;

    float totalDisplacement = 0.0f;

    auto colliders = GetGame()->GetColliders();

    for (auto* other : colliders)
    {
        if (other == this || !other->mIsEnabled)
            continue;

        if (Intersect(*other))
        {
            const float overlapY = GetMinVerticalOverlap(other);

            if (overlapY != 0.0f)
            {
                ResolveVerticalCollisions(rigidBody, overlapY);
                totalDisplacement += overlapY;

                GetOwner()->OnVerticalCollision(overlapY, other);
            }
        }
    }

    return totalDisplacement;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap) const
{
    Vector2 NewPosition(GetOwner()->GetPosition());
    NewPosition.x += minXOverlap;
    GetOwner()->SetPosition(NewPosition);

    if (std::signbit(minXOverlap) != std::signbit(rigidBody->GetVelocity().x))
    {
        Vector2 NewVelocity(rigidBody->GetVelocity());
        NewVelocity.x = 0;
        rigidBody->SetVelocity(NewVelocity);
    }
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap) const
{
    Vector2 NewPosition(GetOwner()->GetPosition());
    NewPosition.y += minYOverlap;
    GetOwner()->SetPosition(NewPosition);

    Vector2 NewVelocity(rigidBody->GetVelocity());
    NewVelocity.y = 0;
    rigidBody->SetVelocity(NewVelocity);

    if (minYOverlap < 0)
        mOwner->SetOnGround();
}

void AABBColliderComponent::DebugDraw(class Renderer *renderer)
{
    renderer->DrawRect(GetMin(),Vector2(mWidth, mHeight), mOwner->GetRotation(),
                       Color::Green, mOwner->GetGame()->GetCameraPos(), RendererMode::LINES);
}

void AABBColliderComponent::Resize(const int w, const int h, const Vector2 offset)
{
    mWidth = w;
    mHeight = h;
    mOffset = offset;
}
