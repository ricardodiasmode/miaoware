//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "SpawnBlock.h"

#include "../Game.h"

void SpawnBlock::OnFinishMovement()
{
    MovingBlock::OnFinishMovement();

    mCanMove = false; // spawn blocks move/spawn only once

    Actor* spawnedActor = spawn();
    Vector2 spawnPosition = GetPosition();
    spawnPosition.y -= Game::TILE_SIZE;
    spawnedActor->SetPosition(spawnPosition);
}
