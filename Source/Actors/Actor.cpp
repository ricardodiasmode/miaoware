    // ----------------------------------------------------------------
    // From Game Programming in C++ by Sanjay Madhav
    // Copyright (C) 2017 Sanjay Madhav. All rights reserved.
    //
    // Released under the BSD License
    // See LICENSE in root directory for full details.
    // ----------------------------------------------------------------

    #include "Actor.h"
    #include "../Game.h"
    #include "../Components/Component.h"
    #include <algorithm>
    #include "../Components/Physics/AABBColliderComponent.h"


    Actor::Actor(Game* game, const std::string& uniqueName)
            : mState(ActorState::Active)
            , mPosition(Vector2::Zero)
            , mScale(Vector2(1.0f, 1.0f))
            , mRotation(0.0f)
            , mGame(game)
            , mActorName(uniqueName)
    {
        mGame->AddActor(this);
    }

    Actor::~Actor()
    {
        mGame->RemoveActor(this);

        for(auto component : mComponents)
        {
            delete component;
        }
        mComponents.clear();
    }

    void Actor::Update(float deltaTime)
    {
        if (mState == ActorState::Active)
        {
            for (auto comp : mComponents)
            {
                if (comp->IsEnabled()) {
                    comp->Update(deltaTime);
                }
            }

            OnUpdate(deltaTime);
        }
    }

    void Actor::OnUpdate(float deltaTime)
    {

    }

    void Actor::ProcessInput(const Uint8* keyState)
    {

        if (mState == ActorState::Active)
        {
            for (auto comp : mComponents)
            {
                if (comp->IsEnabled()) {
                    comp->ProcessInput(keyState);
                }
            }

            OnProcessInput(keyState);
        }
    }

    void Actor::OnProcessInput(const Uint8* keyState)
    {

    }

    void Actor::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {

    }

    void Actor::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {

    }

    void Actor::Kill()
    {

    }

    void Actor::AddComponent(Component* c)
    {
        mComponents.emplace_back(c);
        std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
            return a->GetUpdateOrder() < b->GetUpdateOrder();
        });
    }

    void Actor::SetScale(const Vector2& scale)
    {
        mScale = scale;

        // Repassa a escala para todos os AABBColliderComponents
        for (Component* comp : mComponents)
        {
            auto collider = dynamic_cast<AABBColliderComponent*>(comp);
            if (collider && (mScale.x > 1.0f || mScale.y > 1.0f))

            {
                // Novo tamanho = tamanho original * escala
                SDL_Log("scale: %f", scale.x);
                int newW = static_cast<int>(collider->mOrigWidth * scale.x);
                int newH = static_cast<int>(collider->mOrigHeight * scale.y);
                SDL_Log("Collider before: %d x %d", collider->mWidth, collider->mHeight);

                // Quanto o tamanho aumentou
                float deltaW = newW - collider->mOrigWidth;
                float deltaH = newH - collider->mOrigHeight;

                // Como a origem é central, metade do aumento deve ir para cada lado
                Vector2 newOffset = collider->mOrigOffset - Vector2(deltaW * 0.5f, deltaH * 0.5f);


                collider->Resize(newW, newH, newOffset);

                SDL_Log("Collider after: %d x %d", collider->mWidth, collider->mHeight);

            }
        }
    }



