// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Background.h"
#include "Actors/Block.h"
#include "Actors/MovingBlock.h"
#include "Actors/Goomba.h"
#include "Actors/Spawner.h"
#include "Actors/Mario.h"
#include "Actors/Mushroom.h"
#include "Actors/SpawnBlock.h"
#include "Components/Drawing/AnimatorComponent.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCameraPos(0.f, 0.f)
        ,mMario(nullptr)
        ,mLevelData(nullptr)
{

}

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("miaoware", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::InitializeActors()
{
    //int** level = LoadLevel("../Assets/Levels/Level1-1/level1-1.csv", Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
    int** level = LoadLevel("../Assets/Levels/Level1-2/level1-2.csv", Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
    BuildLevel(level, Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    // Aloca a matriz de inteiros (height x width)
    int** level = new int*[height];
    for (int i = 0; i < height; i++)
    {
        level[i] = new int[width];
    }

    // Abre o arquivo CSV
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Erro ao abrir o arquivo: %s", fileName.c_str());
        return nullptr;
    }

    // Lê o arquivo linha por linha
    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < height)
    {
        // Usa o CSVHelper para separar os valores
        std::vector<int> values = CSVHelper::Split(line, ',');

        // Preenche a linha da matriz
        for (int col = 0; col < width && col < values.size(); col++)
        {
            level[row][col] = values[col];
        }

        row++;
    }

    file.close();

    // Imprime a matriz para verificação
    SDL_Log("=== Level carregado de %s ===", fileName.c_str());
    for (int i = 0; i < height; i++)
    {
        std::string rowStr = "";
        for (int j = 0; j < width; j++)
        {
            rowStr += std::to_string(level[i][j]);
            if (j < width - 1)
                rowStr += ",";
        }
    }

    return level;
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    auto* bg = new Background(this, "../Assets/Sprites/Background.png");
    bg->SetPosition(Vector2(3408, 210));

    // Percorre a matriz de tiles
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            int tileID = levelData[row][col];

            int posX = col * TILE_SIZE;
            int posY = row * TILE_SIZE;
            Block* NewBlock = nullptr;

            // Instancia objetos baseado no ID do tile
            switch (tileID)
            {
                case 0:
                    // Chão
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockA.png");
                    break;
                case 1:
                {
                    // Interrogação
                    // Hardcodando posições porque nao foi especificado como seria a pré-definição (poderia ser em .csv também)
                    static const std::vector<Vector2> spawnableBlocks = {
                        Vector2(2, 34),
                        Vector2(10, 162)
                    };
                    const Vector2 currentPos(row, col);
                    if (std::find(spawnableBlocks.begin(), spawnableBlocks.end(), currentPos) != spawnableBlocks.end())
                    {
                        NewBlock = SpawnBlock::create<Mushroom>(
                        this, "../Assets/Sprites/Blocks/BlockC.png",
                        this, 100.f
                        );
                    } else
                    {
                        NewBlock = new MovingBlock(
                        this, "../Assets/Sprites/Blocks/BlockC.png"
                        );
                    }
                    break;
                }
                case 4:
                    // Brick
                    NewBlock = new MovingBlock(this, "../Assets/Sprites/Blocks/BlockB.png");
                    break;
                case 8:
                    // Chão especial
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockD.png");
                    break;
                case 10:
                {
                    auto* spawner = new Spawner(this, SPAWN_DISTANCE);
                    const Vector2 pos(posX, posY);
                    spawner->SetPosition(pos);
                    break;
                }
                case 12:
                    // Cano verde cima direita
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockG.png");
                    break;
                case 2:
                    // Cano verde cima esquerda
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockF.png");
                    break;
                case 9:
                    // cano verde baixo esquerda
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockH.png");
                    break;
                case 6:
                    // Cano verde baixo direita
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockI.png");
                    break;
                case 7:
                    // Cano verde topo direito
                    NewBlock = new Block(this, "../Assets/Sprites/Blocks/BlockD.png");
                    break;
                case 16:
                {
                    mMario = new Mario(this);
                    const Vector2 pos(posX, posY);
                    mMario->SetPosition(pos);
                }
                default:
                    break;
            }
            if (NewBlock)
            {
                const Vector2 pos(posX, posY);
                NewBlock->SetPosition(pos);
            }
        }
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::UpdateCamera()
{
    if (!mMario)
        return;
    float desiredXLoc = mMario->GetPosition().x - WINDOW_WIDTH/2.f;

    if (desiredXLoc < 0.f)
        desiredXLoc = 0.f;

    Vector2 clampedPos(desiredXLoc, GetCameraPos().y);

    SetCameraPos(clampedPos);
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if(mIsDebugging)
        {
           // Call draw for actor components
              for (auto comp : drawable->GetOwner()->GetComponents())
              {
                comp->DebugDraw(mRenderer);
              }
        }
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete level data
    if (mLevelData) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}