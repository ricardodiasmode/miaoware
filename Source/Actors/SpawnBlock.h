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
    SpawnBlock(Game* game, const std::string& uniqueName, const std::string &texturePath,
                std::function<Actor*()> factory)
        : MovingBlock(game, uniqueName, texturePath), mFactory(std::move(factory))
    {}

protected:
    void OnFinishMovement() override;

public:
    template <typename T, typename... ExtraArgs>
    static SpawnBlock* create(Game* game,
                          const std::string& uniqueName,
                          const std::string& texturePath,
                          ExtraArgs&&... extraArgs)
    {
        // capture extra arguments in a tuple so they keep their true value category
        auto argsTuple = std::make_tuple(std::forward<ExtraArgs>(extraArgs)...);

        auto factory = [=]() mutable {
            return std::apply(
                [&](auto&&... unpacked) {
                    return new T(
                        game,
                        uniqueName,
                        std::forward<decltype(unpacked)>(unpacked)...
                    );
                },
                argsTuple
            );
        };

        return new SpawnBlock(game, uniqueName, texturePath, std::move(factory));
    }


    Actor* spawn() {
        return mFactory();
    }
};
