//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Dog.h"

#include "Cat.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Dog::Dog(Game* game, const std::string& uniqueName, float forwardSpeed, float deathTime)
        : Actor(game, uniqueName)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mDamageEnabled(true)
{
        mDrawComponent = new AnimatorComponent(this,
            "../Assets/Sprites/Dog/Dog.png",
            "../Assets/Sprites/Dog/Dog.json",
            Game::TILE_SIZE,
            Game::TILE_SIZE);

        mDrawComponent->AddAnimation("walk", {1, 2, 3});
        mDrawComponent->AddAnimation("dead", {0});
        mDrawComponent->SetAnimation("walk");
        mDrawComponent->SetAnimFPS(10.f);

        mRigidBodyComponent = new RigidBodyComponent(this);
        Vector2 newVelocity(mForwardSpeed, 0.f);
        mRigidBodyComponent->SetVelocity(newVelocity);

        mColliderComponent = new AABBColliderComponent(this,
                0,
                0,
                Game::TILE_SIZE,
                Game::TILE_SIZE,
                ColliderLayer::Enemy);
        mColliderComponent->SetEnabled(true);

        game->AddDog(this);
}

void Dog::Kill()
{
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Dog::OnUpdate(float deltaTime)
{
    if (mIsDying)
    {
        mDyingTimer -= deltaTime;

        if (mDyingTimer <= 0.f)
            mState = ActorState::Destroy;
    }

    if (GetPosition().y > 45*Game::TILE_SIZE)
        mState = ActorState::Destroy;
}

void Dog::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player && mDamageEnabled)
    {
        dynamic_cast<Cat*>(other->GetOwner())->EnemyHit(mColliderComponent);
        return;
    }

    Vector2 newVelocity = mRigidBodyComponent->GetVelocity();
    newVelocity.x = -newVelocity.x;
    mRigidBodyComponent->SetVelocity(newVelocity);
    float direction = newVelocity.x < 0 ? -1.f : 1.f;
    SetScale(Vector2((direction), 1.f));
}
