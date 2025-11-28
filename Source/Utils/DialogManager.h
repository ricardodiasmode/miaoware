//
// Created by ricar on 11/28/2025.
//

#pragma once
#include <string>

enum class DialogKeys
{
    FASE1,
    FASE2,
    FASE3,
    FASE4,
    FASE5
};

class Terminal;

class DialogManager
{
private:
    Terminal* mTerminal = nullptr;
    std::map<DialogKeys, std::vector<std::string>> mDialogs;

private:
    void LoadDialogs(const std::string& dialogFilePath);

    static DialogKeys DialogKeyFromString(const std::string& key)
    {
        if (key == "FASE1") return DialogKeys::FASE1;
        if (key == "FASE2") return DialogKeys::FASE2;
        if (key == "FASE3") return DialogKeys::FASE3;
        if (key == "FASE4") return DialogKeys::FASE4;
        if (key == "FASE5") return DialogKeys::FASE5;

        throw std::runtime_error("Unknown dialog key: " + key);
    }

public:
    explicit DialogManager(Terminal* terminal, const std::string& dialogFilePath = "../Assets/Dialogs.json") : mTerminal(terminal)
    {
        LoadDialogs(dialogFilePath);
    }

    void PlayDialog(DialogKeys dialogKey);
};
