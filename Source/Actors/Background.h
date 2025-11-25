//
// Created by ricar on 10/19/2025.
//

#pragma once

#include "Actor.h"


class Background : public Actor
{
public:
    explicit Background(Game* game, const std::string& uniqueName, const std::string &texturePath);
};
