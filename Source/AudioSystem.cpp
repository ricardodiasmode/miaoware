#include "AudioSystem.h"
#include <SDL.h>

AudioSystem::AudioSystem() : mNumChannels(16) {}

AudioSystem::~AudioSystem() { Shutdown(); }

bool AudioSystem::Initialize()
{
    int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    if ((Mix_Init(flags) & flags) == 0)
    {
        SDL_Log("Mix_Init failed: %s", Mix_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_Log("Mix_OpenAudio failed: %s", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(mNumChannels);
    return true;
}

void AudioSystem::Shutdown()
{
    for (auto &kv : mChunkCache)
    {
        if (kv.second)
        {
            Mix_FreeChunk(kv.second);
        }
    }
    mChunkCache.clear();

    if (Mix_QuerySpec(nullptr, nullptr, nullptr))
    {
        Mix_CloseAudio();
    }
    Mix_Quit();
}

void AudioSystem::Update()
{
}

Mix_Chunk* AudioSystem::LoadChunk(const std::string& soundName)
{
    auto it = mChunkCache.find(soundName);
    if (it != mChunkCache.end()) return it->second;

    std::string fullPath = std::string("../Assets/Sounds/") + soundName;
    Mix_Chunk* chunk = Mix_LoadWAV(fullPath.c_str());
    if (!chunk)
    {
        SDL_Log("Failed to load sound '%s': %s", fullPath.c_str(), Mix_GetError());
        return nullptr;
    }
    mChunkCache.emplace(soundName, chunk);
    return chunk;
}

int AudioSystem::FindOrStealChannel()
{
    int ch = Mix_PlayChannel(-1, nullptr, 0);
    if (ch >= 0) return ch;

    for (int i = 0; i < mNumChannels; ++i)
    {
        if (Mix_Playing(i))
        {
            Mix_HaltChannel(i);
            return i;
        }
    }
    return 0;
}

void AudioSystem::PlaySound(const std::string& soundName, bool looping)
{
    Mix_Chunk* chunk = LoadChunk(soundName);
    if (!chunk) return;

    Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);

    int loops = looping ? -1 : 0;
    int ch = Mix_PlayChannel(-1, chunk, loops);
    if (ch == -1)
    {
        int steal = FindOrStealChannel();
        ch = Mix_PlayChannel(steal, chunk, loops);
        if (ch == -1)
        {
            SDL_Log("Failed to play sound '%s': %s", soundName.c_str(), Mix_GetError());
        }
    }
    else
    {
        SDL_Log("[Audio] Playing '%s' on channel %d (loops=%d)", soundName.c_str(), ch, loops);
    }
}

void AudioSystem::SetVolume(const std::string& soundName, int volume)
{
    Mix_Chunk* chunk = LoadChunk(soundName);
    if (!chunk) return;
    int v = volume;
    if (v < 0) v = 0;
    if (v > MIX_MAX_VOLUME) v = MIX_MAX_VOLUME;
    Mix_VolumeChunk(chunk, v);
}

void AudioSystem::StopSound(const std::string& /*soundName*/)
{
    for (int i = 0; i < mNumChannels; ++i)
    {
        Mix_HaltChannel(i);
    }
    SDL_Log("[Audio] Stop all channels");
}

void AudioSystem::PauseSound(const std::string& /*soundName*/)
{
    for (int i = 0; i < mNumChannels; ++i)
    {
        if (Mix_Playing(i)) Mix_Pause(i);
    }
}

void AudioSystem::ResumeSound(const std::string& /*soundName*/)
{
    for (int i = 0; i < mNumChannels; ++i)
    {
        Mix_Resume(i);
    }
}
