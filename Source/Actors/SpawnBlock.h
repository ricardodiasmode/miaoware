//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include <functional>

#include "MovingBlock.h"

class SpawnBlock : public MovingBlock
{
    std::function<Actor*()> mFactory;


private:
    // Private constructor
    SpawnBlock(Game* game, const std::string &texturePath,
                std::function<Actor*()> factory)
        : MovingBlock(game, texturePath), mFactory(std::move(factory))
    {}

protected:
    void OnFinishMovement() override;

public:
    template <typename T, typename... Args>
    static SpawnBlock* create(Game* game, const std::string &texturePath, Args&&... args) {
        auto factory = [args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            return std::apply([](auto&&... capturedArgs) {
                return new T(std::forward<decltype(capturedArgs)>(capturedArgs)...);
            }, std::move(args));
        };
        return new SpawnBlock(game, texturePath, std::move(factory));
    }

    Actor* spawn() {
        return mFactory();
    }
};
