//
// Created by ricar on 11/25/2025.
//

#include "ObjectManager.h"
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../Actors/Cat.h"
#include <vector>
#include <algorithm>

ObjectManager::ObjectManager(Game *game) : mGame(game)
{

}

Actor * ObjectManager::GetActorByName(const std::string &actorName) const
{
    std::vector<Actor*> allActors = mGame->GetAllActors();
    for (Actor* actor : allActors)
    {
        if (actor->mIsManageable) {
            std::string actorNameStr = actor->GetActorName();
            std::transform(actorNameStr.begin(), actorNameStr.end(), actorNameStr.begin(), ::tolower);
            if (actorNameStr == actorName)
                return actor;
        }
    }
    return nullptr;
}

std::vector<std::string_view> ObjectManager::GetAllObjNames() const
{
    std::vector<Actor*> allActors = mGame->GetAllActors();
    std::vector<std::string_view> objNames;
    for (Actor* actor : allActors)
    {
        if (actor->mIsManageable)
        {
            std::string_view view(actor->GetActorName());
            objNames.push_back(view);
        }
    }
    return objNames;
}

std::string ObjectManager::GetObjAttributes(const std::string& objName)
{
    Actor* desiredActor = GetActorByName(objName);
    if (!desiredActor)
    {
        SDL_LogError(0, "ObjectManager::GetObjAttributes called with wrong obj name.");
        return {"none"};
    }

    std::string objLocation = "position: (" + std::to_string(desiredActor->GetPosition().x) + "," + std::to_string(desiredActor->GetPosition().y) + ")";
    std::string objRotation = "rotation: " + std::to_string(desiredActor->GetRotation());
    std::string objScale = "scale: (" + std::to_string(desiredActor->GetScale().x) + "," + std::to_string(desiredActor->GetScale().y) + ")";

    // todo
    //std::string objDamage = "damage: " + desiredActor->GetDamage();

    return objLocation + "\n" + objRotation + "\n" + objScale;
}

void ObjectManager::SetAttributeValue(const std::string& objName, const std::string& attributeName, const std::string& value)
{
    Actor* actorToModify = GetActorByName(objName);
    if (!actorToModify)
    {
        SDL_LogError(0, "ObjectManager::SetAttributeValue called with wrong obj name.");
        return;
    }

    // For position, it is expected value to be in the format: (x, y)
    if (attributeName == "position")
    {
        auto inner = value.substr(1, value.size() - 2);
        auto comma = inner.find(',');
        const int x = std::stoi(inner.substr(0, comma));
        const int y = std::stoi(inner.substr(comma + 1));
        const Vector2 newPos(x, y);
        SDL_Log("setting obj position to: %d, %d", x, y);
        actorToModify->SetPosition(newPos);
        return;
    }
    // For rotation, it is expected value to be in the format: x
    if (attributeName == "rotation")
    {
        actorToModify->SetRotation(value.at(0) - '0');
        return;
    }
    // For scale, it is expected value to be in the format: (x, y)
    if (attributeName == "scale")
    {
        auto inner = value.substr(1, value.size() - 2);
        auto comma = inner.find(',');
        const int x = std::stoi(inner.substr(0, comma));
        const int y = std::stoi(inner.substr(comma + 1));
        const Vector2 newScale(x, y);
        actorToModify->SetScale(newScale);
        return;
    }
}

void ObjectManager::Jump()
{
    mGame->GetPlayer()->Jump();
}

void ObjectManager::DeleteObject(const std::string& objName)
{
    Actor* actorToDelete = GetActorByName(objName);
    if (!actorToDelete)
    {
        SDL_LogError(0, "ObjectManager::DeleteObject called with wrong obj name.");
        return;
    }

    actorToDelete->SetState(ActorState::Destroy);
}

std::string ObjectManager::AddObject(const Vector2& playerRelativeLocation, const SpawnableObjects objectToAdd)
{
    const Vector2 spawnPosition = mGame->GetPlayer()->GetPosition() + playerRelativeLocation;

    switch (objectToAdd)
    {
        case SpawnableObjects::Block:
        {
            // here we would spawn something at spawnPosition and return the spawned obj name
            return {};
        }
        default: break;
    }
    return {"none"};
}
