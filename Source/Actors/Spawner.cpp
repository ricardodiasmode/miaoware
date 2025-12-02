//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Cat.h"
#include "Dog.h"
#include "../Components/Physics/AABBColliderComponent.h"

Spawner::Spawner(Game* game, const std::string& uniqueName, float spawnDistance)
        :Actor(game, uniqueName)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
        if (mGame->GetPlayer()->GetPosition().x < mSpawnDistance)
        {
                auto* dog = new Dog(mGame, "Dog" + std::to_string(mGame->GetDogNum()));
                dog->mIsManageable = true;
                dog->SetPosition(GetPosition());
                dog->GetComponent<AABBColliderComponent>()->SetEnabled(true);
                mState = ActorState::Destroy;
        }
}