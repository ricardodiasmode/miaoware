// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <vector>
#include "Renderer/Renderer.h"
#include <algorithm>
#include <functional>

enum class GameScene
{
    MainMenu,
    Level1,
    Level2,
    Level3,
    Level4,
    Level5
};

class Dog;

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor *actor);
    void RemoveActor(class Actor *actor);

    // Renderer
    class Renderer *GetRenderer() { return mRenderer; }

    // Scene Handling
    void SetScene(GameScene scene);
    void UnloadScene();

    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 720;
    static const int LEVEL_WIDTH = 215;
    static const int LEVEL_HEIGHT = 15;
    static const int TILE_SIZE = 32;
    static const int SPAWN_DISTANCE = 700;
    static const int FPS = 60;

    // Draw functions
    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);
    std::vector<class DrawComponent *> &GetDrawables() { return mDrawables; }

    // Collider functions
    void AddCollider(class AABBColliderComponent *collider);
    void RemoveCollider(class AABBColliderComponent *collider);
    std::vector<class AABBColliderComponent *> &GetColliders() { return mColliders; }

    // Camera functions
    Vector2 &GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2 &position) { mCameraPos = position; };

    // Game specific
    class Cat *GetPlayer() { return mCat; }
    class Terminal *GetTerminal() { return mTerminal; }

    std::vector<Actor *> GetAllActors() const { return mActors; }

    GameScene mCurrentScene = GameScene::MainMenu;

    class Font* mUiFont = nullptr;
    class AudioSystem* mAudio = nullptr;
    class MainMenu* mMainMenu = nullptr;

    void AddDog(Dog* dog) { mDogs.push_back(dog); }
    void RemoveDog(Dog* dog) {
        auto it = std::find(mDogs.begin(), mDogs.end(), dog);
        if (it != mDogs.end())
            *it = mDogs.back(), mDogs.pop_back();  // swap-with-last
    }
    int GetDogNum() const { return static_cast<int>(mDogs.size()); }

    void RestartLevel();

private:
    void InitializeCore();

    void UnloadMenu();

    void ProcessInput();

    void GoToNextScene();

    void UpdateGame(float deltaTime);
    void UpdateCamera();
    void GenerateOutput();

    // Level loading
    int **LoadLevel(const std::string &fileName, int width, int height);
    void BuildLevel(int **levelData, int width, int height);

    bool ValidateVector2(const std::string & string);
    bool ValidateNumber(const std::string & string);

    void ProcessTerminalCommand(const std::string &input);

    void SetConditionForLevelChange(std::function<bool()> Condition)
    {
        mLevelChangeCondition = std::move(Condition);
    }


    std::function<bool()> mLevelChangeCondition;

    // All the actors in the game
    std::vector<class Actor *> mActors;
    std::vector<class Actor *> mPendingActors;
    std::vector<class Dog *> mDogs;

    // Camera
    Vector2 mCameraPos;

    // All the draw components
    std::vector<class DrawComponent *> mDrawables;

    // All the collision components
    std::vector<class AABBColliderComponent *> mColliders;

    // SDL stuff
    SDL_Window *mWindow;
    class Renderer *mRenderer;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;

    // Game-specific
    class Cat *mCat;
    int **mLevelData;

    class ObjectManager *mObjManager = nullptr;
    class Terminal *mTerminal;
    class DialogManager *mDialogManager = nullptr;


    // fade
    bool mIsFadeIn = false;
    bool mIsFading = false;
    std::function<void()> mFadeCallback;
    void Fade(const float deltaTime);
    void StartFade(const std::function<void()>& fadeCallback);

public:
    float mFadeValue = 0.f;
};
