//
// Created by ricar on 10/19/2025.
//

#include "Background.h"

#include "../Game.h"
#include "../Components/Drawing/TextureComponent.h"

Background::Background(Game *game, const std::string& uniqueName, const std::string &texturePath)
        :Actor(game, uniqueName)
{
        new TextureComponent(this,
                texturePath,
                6784,
                448,
                99);
}
