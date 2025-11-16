//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"

#include "MovingBlock.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
{
    mDrawComponent = new AnimatorComponent(this,
        "../Assets/Sprites/Mario/Mario.png",
        "../Assets/Sprites/Mario/Mario.json",
        Game::TILE_SIZE,
        Game::TILE_SIZE);
    mDrawComponent->AddAnimation("idle", {1});
    mDrawComponent->AddAnimation("jump", {2});
    mDrawComponent->AddAnimation("run", {3, 4, 5});
    mDrawComponent->AddAnimation("dead", {0});

    mDrawComponentBig = new AnimatorComponent(this,
        "../Assets/Sprites/SuperMario/SuperMario.png",
        "../Assets/Sprites/SuperMario/SuperMario.json",
        Game::TILE_SIZE,
        Game::TILE_SIZE*2,
        0.f,
        16.f);
    mDrawComponentBig->AddAnimation("idle", {0});
    mDrawComponentBig->AddAnimation("jump", {2});
    mDrawComponentBig->AddAnimation("run", {6, 7, 8});   // RunLeft0, RunLeft1, RunLeft2
    mDrawComponentBig->SetAnimFPS(10.f);
    mDrawComponentBig->SetEnabled(false);
    mDrawComponentBig->SetVisible(false);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.f, 5.f);

    mColliderComponent = new AABBColliderComponent(this,
            0,
            0,
            Game::TILE_SIZE,
            Game::TILE_SIZE,
            ColliderLayer::Player);
}

void Mario::OnProcessInput(const uint8_t* state)
{
    if (state[SDL_SCANCODE_A])
    {
        const Vector2 speed(-mForwardSpeed, 0.f);
        mRigidBodyComponent->ApplyForce(speed);

        const Vector2 scale(-1.f, 1.f);
        SetScale(scale);
        mIsRunning = true;
    } else if (state[SDL_SCANCODE_D])
    {
        const Vector2 Speed(mForwardSpeed, 0.f);
        mRigidBodyComponent->ApplyForce(Speed);

        const Vector2 scale(1.f, 1.f);
        SetScale(scale);
        mIsRunning = true;
    } else
    {
        mIsRunning = false;
    }

    if (state[SDL_SCANCODE_SPACE] && mIsOnGround)
    {
        Vector2 newVelocity = mRigidBodyComponent->GetVelocity();
        newVelocity.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(newVelocity);
    }
}

void Mario::OnUpdate(float deltaTime)
{
    if (GetPosition().x < 0.f)
    {
        Vector2 posToSet(0.f, GetPosition().y);
        SetPosition(posToSet);
    }
    if (GetPosition().y > Game::LEVEL_HEIGHT*Game::TILE_SIZE)
    {
        Kill();
    }

    if (mRigidBodyComponent->GetVelocity().y != 0)
        mIsOnGround = false;


    ManageAnimations();
}

void Mario::ManageAnimations()
{
    if (mIsDead)
        return;

    if (mIsOnGround)
    {
        if (mIsRunning)
            (mIsBig ? mDrawComponentBig : mDrawComponent)->SetAnimation("run");
        else
            (mIsBig ? mDrawComponentBig : mDrawComponent)->SetAnimation("idle");
    }
    else
        (mIsBig ? mDrawComponentBig : mDrawComponent)->SetAnimation("jump");
}

void Mario::Kill()
{
    DecreaseSize();

    mDrawComponent->SetAnimation("dead");
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Mario::EatMushroomEffect()
{
    mIsBig = true;
    mDrawComponent->SetEnabled(false);
    mDrawComponent->SetVisible(false);
    mDrawComponentBig->SetEnabled(true);
    mDrawComponentBig->SetVisible(true);
    constexpr Vector2 newOffset(0, 0);
    mColliderComponent->Resize(32, 64, newOffset);
}

void Mario::DecreaseSize()
{
    if (!mIsBig)
        return;

    mIsBig = false;
    mDrawComponent->SetEnabled(true);
    mDrawComponent->SetVisible(true);
    mDrawComponentBig->SetEnabled(false);
    mDrawComponentBig->SetVisible(false);

    constexpr Vector2 newOffset(0, 0);
    mColliderComponent->Resize(32, 32, newOffset);
}

void Mario::EnemyHit(AABBColliderComponent *other)
{
    if (mIsBig)
    {
        DecreaseSize();

        other->GetOwner()->Kill();
    }
    else
    {
        Kill();
    }
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        EnemyHit(other);
    } else if (other->GetLayer() == ColliderLayer::Mushroom)
    {
        other->GetOwner()->SetState(ActorState::Destroy);
        EatMushroomEffect();
    }
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        other->GetOwner()->Kill();

        Vector2 newVelocity = mRigidBodyComponent->GetVelocity();
        newVelocity.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(newVelocity);
        return;
    } else if (other->GetLayer() == ColliderLayer::Blocks &&
        dynamic_cast<MovingBlock*>(other->GetOwner()) &&
        minOverlap > 0)
    {
        dynamic_cast<MovingBlock*>(other->GetOwner())->StartMovementInterp();
    } else if (other->GetLayer() == ColliderLayer::Mushroom)
    {
        other->GetOwner()->SetState(ActorState::Destroy);
        EatMushroomEffect();
    }
}
