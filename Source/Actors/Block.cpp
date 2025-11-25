//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Block::Block(Game* game, const std::string& uniqueName, const std::string &texturePath, const bool isStatic)
        :Actor(game, uniqueName)
{
        new AnimatorComponent(this,
                texturePath,
                {},
                Game::TILE_SIZE,
                Game::TILE_SIZE);

        new AABBColliderComponent(this,
                0,
                0,
                Game::TILE_SIZE,
                Game::TILE_SIZE,
                ColliderLayer::Blocks,
                isStatic);
}
