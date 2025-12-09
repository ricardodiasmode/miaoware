//
// Created by ricar on 11/25/2025.
//

#include "ObjectManager.h"
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../Terminal.h"
#include "../Actors/Cat.h"
#include "../Actors/Block.h"
#include "../Actors/Dog.h"
#include <vector>
#include <algorithm>

#include "TerminalHelper.h"

ObjectManager::ObjectManager(Game *game) : mGame(game)
{
}

Actor *ObjectManager::GetActorByName(std::string actorName) const
{
    std::vector<Actor *> allActors = mGame->GetAllActors();
    for (Actor *actor : allActors)
    {
        if (actor->mIsManageable)
        {
            std::string actorNameStr = actor->GetActorName();
            std::transform(actorNameStr.begin(), actorNameStr.end(), actorNameStr.begin(), ::tolower);
            std::transform(actorName.begin(), actorName.end(), actorName.begin(), ::tolower);
            if (actorNameStr == actorName)
                return actor;
        }
    }
    return nullptr;
}

std::vector<std::string_view> ObjectManager::GetAllObjNames() const
{
    std::vector<Actor *> allActors = mGame->GetAllActors();
    std::vector<std::string_view> objNames;
    for (Actor *actor : allActors)
    {
        if (actor->mIsManageable)
        {
            std::string_view view(actor->GetActorName());
            objNames.push_back(view);
        }
    }
    return objNames;
}

std::string ObjectManager::GetObjAttributes(const std::string &objName)
{
    Actor *desiredActor = GetActorByName(objName);
    if (!desiredActor)
    {
        SDL_LogError(0, "ObjectManager::GetObjAttributes called with wrong obj name: %s.", objName.c_str());
        return {"none"};
    }

    std::string objLocation = "position: (" + std::to_string(desiredActor->GetPosition().x) + "," + std::to_string(desiredActor->GetPosition().y) + ")";
    std::string objRotation = "rotation: " + std::to_string(desiredActor->GetRotation());
    std::string objScale = "scale: (" + std::to_string(desiredActor->GetScale().x) + "," + std::to_string(desiredActor->GetScale().y) + ")";

    // todo
    // std::string objDamage = "damage: " + desiredActor->GetDamage();

    return objLocation + "\n" + objRotation + "\n" + objScale;
}


void ObjectManager::SetAttributeValue(const std::string &objName, const std::string &attributeName, const std::string &value)
{
    Actor *actorToModify = GetActorByName(objName);
    if (!actorToModify)
    {
        SDL_LogError(0, "ObjectManager::SetAttributeValue called with wrong obj name: %s.", objName.c_str());
        return;
    }

    // For position, it is expected value to be in the format: (x, y)
    if (MatchesCmd(attributeName, "position"))
    {
        auto inner = value.substr(1, value.size() - 2);
        auto comma = inner.find(',');
        const int x = std::stoi(inner.substr(0, comma));
        const int y = std::stoi(inner.substr(comma + 1));
        // Verifica se a posição solicitada está dentro dos limites visíveis pela câmera
        Vector2 cam = mGame->GetCameraPos();
        float camLeft = cam.x;
        float camTop = cam.y;
        float camRight = camLeft + Game::WINDOW_WIDTH;
        float camBottom = camTop + Game::WINDOW_HEIGHT;

        // if (x < camLeft || x > camRight || y < camTop || y > camBottom)
        // {
        //     mGame->GetTerminal()->AddLine("Error: Requested position is outside camera bounds.");
        //     SDL_Log("ObjectManager: requested position (%d,%d) is outside camera bounds; change ignored", x, y);
        //     SDL_Log("camLeft: %d camRight: %d camTop: %d camBottom: %d", camLeft, camRight,camTop, camBottom);
        //     return;
        // }

        const Vector2 newPos(static_cast<float>(x), static_cast<float>(y));
        SDL_Log("setting obj position to: %d, %d", x, y);
        actorToModify->SetPosition(newPos);
        return;
    }
    // For rotation, it is expected value to be in the format: x
    if (MatchesCmd(attributeName, "rotation"))
    {
        actorToModify->SetRotation(value.at(0) - '0');
        return;
    }
    // For scale, it is expected value to be in the format: (x, y)
    if (MatchesCmd(attributeName, "scale"))
    {
        auto inner = value.substr(1, value.size() - 2);
        auto comma = inner.find(',');
        const int x = std::stoi(inner.substr(0, comma));
        const int y = std::stoi(inner.substr(comma + 1));
        const Vector2 newScale(x, y);
        actorToModify->SetScale(newScale);
        return;
    }
    if (attributeName == "damage")
    {
        if (value == "0")
        {
            SDL_Log("disabling damage");
            DisableDamage(objName);
        } else
        {
            SDL_Log("enabling damage");
            EnableDamage(objName);
        }
        return;
    }
}

void ObjectManager::Jump()
{
    mGame->GetPlayer()->Jump();
}

void ObjectManager::DeleteObject(const std::string &objName)
{
    Actor *actorToDelete = GetActorByName(objName);
    if (!actorToDelete)
    {
        SDL_LogError(0, "ObjectManager::DeleteObject called with wrong obj name.");
        return;
    }

    actorToDelete->SetState(ActorState::Destroy);
}

void ObjectManager::DisableDamage(const std::string &objName)
{
    Actor *actorToDelete = GetActorByName(objName);
    Dog* dogToDelete = dynamic_cast<Dog*>(actorToDelete);
    if (!dogToDelete)
    {
        SDL_LogError(0, "ObjectManager::RemoveDamage called with wrong obj name.");
        return;
    }

    dogToDelete->DisableDamage();
}

void ObjectManager::EnableDamage(const std::string &objName)
{
    Actor *actorToDelete = GetActorByName(objName);
    Dog* dogToDelete = dynamic_cast<Dog*>(actorToDelete);
    if (!dogToDelete)
    {
        SDL_LogError(0, "ObjectManager::RemoveDamage called with wrong obj name.");
        return;
    }

    dogToDelete->EnableDamage();
}

std::string ObjectManager::AddObject(const Vector2 &playerRelativeLocation, const SpawnableObjects objectToAdd)
{
    const Vector2 spawnPosition = mGame->GetPlayer()->GetPosition() + playerRelativeLocation;

    switch (objectToAdd)
    {
    case SpawnableObjects::Block:
    { // example spawn
        auto *newBlock = new Block(mGame, "Block" + std::to_string(9999), "../Assets/Sprites/Blocks/BlockI.png");
        newBlock->SetPosition(spawnPosition);
        return {};
    }
    default:
        break;
    }
    return {"none"};
}
