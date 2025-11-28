//
// Created by ricar on 11/25/2025.
//

#pragma once
#include <string>
#include <vector>

#include "../Math.h"

class Game;
class Actor;

enum class SpawnableObjects
{
    Block
};

// Singleton that manages the console commands
class ObjectManager
{
    Game* mGame = nullptr;

private:
    Actor* GetActorByName(std::string actorName) const;

public:
    ObjectManager(Game* game);

    // Returns all names for objects that ARE MANAGEABLE.
    std::vector<std::string_view> GetAllObjNames() const;

    std::string GetObjAttributes(const std::string& objName);
    void SetAttributeValue(const std::string& objName, const std::string& attributeName, const std::string& value);
    void Jump();
    void DeleteObject(const std::string& objName);

    /**
     * @param playerRelativeLocation The location that the object will spawn, relative to the player
     * @param objectToAdd The type of the object that will be spawned
     * @return The name of the spawned object
     */
    std::string AddObject(const Vector2& playerRelativeLocation, const SpawnableObjects objectToAdd);

};
