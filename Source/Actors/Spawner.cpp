//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"
#include "../Components/Physics/AABBColliderComponent.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
        if (mGame->GetPlayer()->GetPosition().x < mSpawnDistance)
        {
                auto* goomba = new Goomba(mGame);

                goomba->SetPosition(GetPosition());

                goomba->GetComponent<AABBColliderComponent>()->SetEnabled(true);

                mState = ActorState::Destroy;
        }
}