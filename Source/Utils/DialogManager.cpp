//
// Created by ricar on 11/28/2025.
//

#include "../Json.h"
#include "DialogManager.h"

#include <fstream>

#include "../Terminal.h"

void DialogManager::LoadDialogs(const std::string &dialogFilePath)
{
    // Load JSON from file
    std::ifstream file(dialogFilePath);
    if (!file.is_open())
        throw std::runtime_error("Failed to open dialog file: " + dialogFilePath);

    nlohmann::json jsonData;
    file >> jsonData;

    // Parse dialogs
    for (const auto& entry : jsonData["dialogs"])
    {
        // Convert "FASE1" string to enum
        DialogKeys key = DialogKeyFromString(entry["id"].get<std::string>());

        // Extract lines
        std::vector<std::string> lines;
        for (const auto& lineObj : entry["lines"])
        {
            lines.push_back(lineObj["text"].get<std::string>());
        }

        // Store in map
        mDialogs[key] = std::move(lines);
    }
}

void DialogManager::PlayDialog(const DialogKeys dialogKey)
{
    const std::vector<std::string>& dialogLines = mDialogs.at(dialogKey);

    for (const std::string& currentLine : dialogLines)
        mTerminal->AddLine(currentLine);
}
