//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Cat.h"

#include "Dog.h"
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
        "../Assets/Sprites/Cat/Cat.png",
        "../Assets/Sprites/Cat/Cat.json",
        Game::TILE_SIZE,
        Game::TILE_SIZE);
    mDrawComponent->AddAnimation("idle", {0});
    mDrawComponent->AddAnimation("jump", {2,4,5,1,6,9,3,7});
    mDrawComponent->AddAnimation("run", {10,16,18,14,19,20,21,22}); // Select a smooth subset for looping
    mDrawComponent->SetAnimFPS(6.f); // base (jump speed)

    mDrawComponent->SetAnimation("idle");

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
    if (mCanJump) {
        Vector2 newVelocity = mRigidBodyComponent->GetVelocity();
        newVelocity.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(newVelocity);

        if (mGame && mGame->mAudio)
        {
            mGame->mAudio->PlaySound("Cat/Jump.wav", false);
            mGame->mAudio->SetVolume("Cat/Jump.wav", 96);
        }
    }
    mCanJump = false;
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
    if (GetPosition().y > 45*Game::TILE_SIZE)
    {
        if (!mIsDead)
            Kill();
    }

    if (mRigidBodyComponent->GetVelocity().y != 0 )
        mIsOnGround = false;

    ManageAnimations();

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
        {
            mDrawComponent->SetAnimFPS(10.f);
            mDrawComponent->SetAnimation("run");
        }
        else
        {
            mDrawComponent->SetAnimFPS(4.f);
            mDrawComponent->SetAnimation("idle");
        }
    }
    else
    {
        mDrawComponent->SetAnimFPS(6.f);
        mDrawComponent->SetAnimation("jump");
    }
}

void Cat::Kill()
{
    if (mIsDead)
        return;

    mDrawComponent->SetAnimation("dead");
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    if (mGame && mGame->mAudio)
    {
        mGame->mAudio->StopSound("Levels/BackgroundMusic.wav");
        mGame->mAudio->StopSound("Cat/Walking.wav");
        mGame->mAudio->PlaySound("MainMenu/Screaming.wav", false);
        mGame->mAudio->SetVolume("MainMenu/Screaming.wav", 96);
    }

    if (mGame)
        mGame->RestartLevel();
}

void Cat::EnemyHit(AABBColliderComponent *other)
{
    Kill();
}

void Cat::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy && dynamic_cast<Dog*>(other->GetOwner())->IsDamageEnabled())
    {
        EnemyHit(other);
    } else if (mAutoWalk){
        ReverseDirection();
    }

    mCanJump = true;
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
    }

    if (other->GetLayer() == ColliderLayer::Blocks &&
        dynamic_cast<MovingBlock*>(other->GetOwner()) &&
        minOverlap > 0)
    {
        dynamic_cast<MovingBlock*>(other->GetOwner())->StartMovementInterp();
    }

    if (minOverlap < 0)
    {
        mCanJump = true;
    }


}

void Cat::ReverseDirection()
{
    mDirection *= -1;
    // imediatamente ajustar velocidade horizontal
    if (mRigidBodyComponent)
    {
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.x = static_cast<float>(mDirection) * mForwardSpeed;
        mRigidBodyComponent->SetVelocity(vel);
    }
    // ajustar sprite
    SetScale(Vector2((mDirection < 0 ? -1.f : 1.f), 1.f));
}
