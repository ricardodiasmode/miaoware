//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Mushroom.h"

#include "Mario.h"
#include "../Game.h"
#include "../Components/Drawing/TextureComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Mushroom::Mushroom(Game* game, float forwardSpeed)
        : Actor(game)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{
        mDrawComponent = new TextureComponent(this,
            "../Assets/Sprites/Collectables/Mushroom.png",
            Game::TILE_SIZE,
            Game::TILE_SIZE);

        mRigidBodyComponent = new RigidBodyComponent(this);
        Vector2 newVelocity(mForwardSpeed, 0.f);
        mRigidBodyComponent->SetVelocity(newVelocity);

        mColliderComponent = new AABBColliderComponent(this,
                0,
                0,
                Game::TILE_SIZE,
                Game::TILE_SIZE,
                ColliderLayer::Mushroom);
}

void Mushroom::Kill()
{
    mState = ActorState::Destroy;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Mushroom::OnUpdate(float deltaTime)
{
    if (GetPosition().y > Game::LEVEL_WIDTH*Game::TILE_SIZE || GetPosition().x < 0.f || GetPosition().x > Game::LEVEL_WIDTH*Game::TILE_SIZE)
        mState = ActorState::Destroy;
}

void Mushroom::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    Vector2 newVelocity = mRigidBodyComponent->GetVelocity();
    newVelocity.x = -newVelocity.x;
    mRigidBodyComponent->SetVelocity(newVelocity);
}

void Mushroom::OnVerticalCollision(float minOverlap, AABBColliderComponent *other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        SetState(ActorState::Destroy);
        dynamic_cast<Mario*>(other->GetOwner())->EatMushroomEffect();
    }
}
