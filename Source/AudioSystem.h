#pragma once
#include <string>
#include <unordered_map>
#include <SDL_mixer.h>

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    bool Initialize();
    void Shutdown();
    void Update();

    void PlaySound(const std::string& soundName, bool looping);
    void StopSound(const std::string& soundName);
    void PauseSound(const std::string& soundName);
    void ResumeSound(const std::string& soundName);

private:
    Mix_Chunk* LoadChunk(const std::string& soundName);
    int FindOrStealChannel();

    std::unordered_map<std::string, Mix_Chunk*> mChunkCache;
    int mNumChannels;
};
