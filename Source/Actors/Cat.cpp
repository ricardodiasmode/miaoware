//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Cat.h"

#include "MovingBlock.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"
#include "../AudioSystem.h"

Cat::Cat(Game* game, const std::string& uniqueName, const float forwardSpeed, const float jumpSpeed)
        : Actor(game, uniqueName)
        , mIsRunning(false)
        , mIsDead(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mDirection(1)
        , mAutoWalk(true)
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

    mIsManageable = true;
}

void Cat::Jump()
{
    Vector2 newVelocity = mRigidBodyComponent->GetVelocity();
    newVelocity.y = mJumpSpeed;
    mRigidBodyComponent->SetVelocity(newVelocity);
}

void Cat::OnProcessInput(const uint8_t* state)
{
    if (mAutoWalk)
    {
        mIsRunning = true;
        return;
    }

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
        Jump();
    }
}

void Cat::OnUpdate(float deltaTime)
{
    if (mAutoWalk && mRigidBodyComponent)
    {
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.x = mDirection * mForwardSpeed;
        mRigidBodyComponent->SetVelocity(vel);
        mIsRunning = true;

        // ajustar escala pra "olhar" direção correta
        if (mDirection < 0)
            SetScale(Vector2(-1.f, 1.f));
        else
            SetScale(Vector2(1.f, 1.f));
    }

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

    // Walking SFX: loop while running on ground; stop/reset otherwise
    if (mGame && mGame->mAudio)
    {
        if (mIsRunning && mIsOnGround)
        {
            if (!mWalkingSfxPlaying)
            {
                SDL_Log("[Cat] Start walking SFX (running=%d, onGround=%d)", (int)mIsRunning, (int)mIsOnGround);
                mGame->mAudio->PlaySound("Cat/Walking.wav", true);
                mWalkingSfxPlaying = true;
            }
        }
        else if (mWalkingSfxPlaying)
        {
            SDL_Log("[Cat] Stop walking SFX (running=%d, onGround=%d)", (int)mIsRunning, (int)mIsOnGround);
            mGame->mAudio->StopSound("Cat/Walking.wav");
            mWalkingSfxPlaying = false;
        }
    }
}

void Cat::ManageAnimations()
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

void Cat::Kill()
{
    DecreaseSize();

    mDrawComponent->SetAnimation("dead");
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Cat::EatMushroomEffect()
{
    mIsBig = true;
    mDrawComponent->SetEnabled(false);
    mDrawComponent->SetVisible(false);
    mDrawComponentBig->SetEnabled(true);
    mDrawComponentBig->SetVisible(true);
    constexpr Vector2 newOffset(0, 0);
    mColliderComponent->Resize(32, 64, newOffset);
}

void Cat::DecreaseSize()
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

void Cat::EnemyHit(AABBColliderComponent *other)
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

void Cat::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        EnemyHit(other);
    } else if (other->GetLayer() == ColliderLayer::Mushroom)
    {
        other->GetOwner()->SetState(ActorState::Destroy);
        EatMushroomEffect();
    } else if (mAutoWalk){
        ReverseDirection();
    }
}

void Cat::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
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

void Cat::ReverseDirection()
{
    mDirection *= -1;
    // imediatamente ajustar velocidade horizontal
    if (mRigidBodyComponent)
    {
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.x = mDirection * mForwardSpeed;
        mRigidBodyComponent->SetVelocity(vel);
    }
    // ajustar sprite
    SetScale(Vector2((mDirection < 0 ? -1.f : 1.f), 1.f));
}
