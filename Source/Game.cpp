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

#include <sstream>
#include <SDL_ttf.h>

#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Terminal.h"
#include "Actors/Actor.h"
#include "Actors/Background.h"
#include "Actors/Block.h"
#include "Actors/MovingBlock.h"
#include "Actors/Spawner.h"
#include "Actors/Cat.h"
#include "Actors/SpawnBlock.h"
#include "Utils/DialogManager.h"
#include "Utils/ObjectManager.h"
#include "Renderer/Font.h"
#include "AudioSystem.h"
#include "MainMenu.h"
#include "Actors/Dog.h"

Game::Game()
    : mWindow(nullptr), mRenderer(nullptr), mTicksCount(0), mIsRunning(true), mIsDebugging(false), mUpdatingActors(false), mCameraPos(0.f, 0.f), mCat(nullptr), mLevelData(nullptr), mTerminal(nullptr), mCurrentScene(GameScene::MainMenu), mUiFont(nullptr), mAudio(nullptr)
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

    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("miaoware", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow, this);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mUiFont = new Font();
    mUiFont->Load("../Assets/Fonts/Arial.ttf");

    mAudio = new AudioSystem();
    if (!mAudio->Initialize())
    {
        SDL_Log("AudioSystem failed to initialize");
    }

    mMainMenu = new MainMenu(this, mUiFont);

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::UnloadScene()
{
    UnloadMenu();

    // Use state so we can call this from withing an a actor update
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

    if (mObjManager)
    {
        delete mObjManager;
        mObjManager = nullptr;
    }
    if (mTerminal)
    {
        delete mTerminal;
        mTerminal = nullptr;
    }
    if (mDialogManager)
    {
        delete mDialogManager;
        mDialogManager = nullptr;
    }
}

void Game::StartFade(const std::function<void()>& fadeCallback)
{
    mFadeCallback = fadeCallback;
    mIsFading = true;
    mFadeValue = 0.f;
    mIsFadeIn = true;
}

void Game::Fade(const float deltaTime)
{
    mFadeValue += mIsFadeIn ? deltaTime : -deltaTime;
    mFadeValue = Math::Clamp(mFadeValue, 0.f, 1.f);
    if (mFadeValue == 0.f || mFadeValue == 1.f)
    {
        if (mFadeValue == 1.f)
        {
            mIsFadeIn = false;
            mFadeCallback();
        } else
        {
            mIsFading = false;
        }
    }
}

void Game::SetScene(GameScene nextScene)
{
    UnloadScene();

    mCurrentScene = nextScene;

    switch (nextScene)
    {
        case GameScene::Level1:
        {
            StartFade([this]
            {
                // Todo: initialize Level1
                int **level = LoadLevel("../Assets/Levels/Level2/level2.csv", 15, 45);
                BuildLevel(level, 15, 45);

                InitializeCore();

                mDialogManager->PlayDialog(DialogKeys::FASE1);

                // todo: condition for level change
                SetConditionForLevelChange([this]
                {
                    if (mCat)
                        return mCat->GetPosition().x > TILE_SIZE*15;
                    return false;
                });
            });
            break;
        }
        case GameScene::Level2:
        {
            StartFade([this]
            {
                int **level = LoadLevel("../Assets/Levels/Level2/level2.csv", 15, 45);
                BuildLevel(level, 15, 45);

                InitializeCore();

                mDialogManager->PlayDialog(DialogKeys::FASE2);

                SetConditionForLevelChange([this]
                {
                    if (mCat)
                        return mCat->GetPosition().x > TILE_SIZE*15;
                    return false;
                });
            });
            break;
        }
        case GameScene::Level3:
        {
            StartFade([this]
            {
                // Todo: initialize Level3
                int **level = LoadLevel("../Assets/Levels/Level3/level3.csv", 45, 15);
                BuildLevel(level, 45, 15);

                InitializeCore();

                mDialogManager->PlayDialog(DialogKeys::FASE3);

                // todo: condition for level change
                SetConditionForLevelChange([this]
                {
                    if (mCat)
                        return mCat->GetPosition().x > TILE_SIZE*45;
                    return false;
                });
            });
            break;
        }
        case GameScene::Level4:
        {
            StartFade([this]
            {
                // Todo: initialize Level4
                int **level = LoadLevel("../Assets/Levels/Level2/level2.csv", 15, 45);
                BuildLevel(level, 15, 45);

                InitializeCore();

                mDialogManager->PlayDialog(DialogKeys::FASE4);

                // todo: condition for level change
                SetConditionForLevelChange([this]
                {
                    if (mCat)
                        return mCat->GetPosition().x > TILE_SIZE*15;
                    return false;
                });
            });
            break;
        }
        case GameScene::Level5:
        {
            StartFade([this]
            {
                // Todo: initialize Level5
                int **level = LoadLevel("../Assets/Levels/Level2/level2.csv", 15, 45);
                BuildLevel(level, 15, 45);

                InitializeCore();

                mDialogManager->PlayDialog(DialogKeys::FASE5);

                // todo: condition for level change
                SetConditionForLevelChange([this]
                {
                    if (mCat)
                        return mCat->GetPosition().x > TILE_SIZE*15;
                    return false;
                });
            });
            break;
        }
        default:
            break;
    }
}

void Game::InitializeActors()
{
}

int **Game::LoadLevel(const std::string &fileName, int width, int height)
{
    // Aloca a matriz de inteiros (height x width)
    int **level = new int *[height];
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

void Game::BuildLevel(int **levelData, int width, int height)
{
    auto *bg = new Background(this, "Background", "../Assets/Sprites/Background.jpg");
    bg->SetPosition(Vector2(3408, 210));

    // Percorre a matriz de tiles
    int objNum = 0;
    int managebleCounter = 0;
    for (int col = 0; col < width; col++)
    {
        for (int row = 0; row < height; row++)

        {
            int tileID = levelData[row][col];

            int posX = col * TILE_SIZE;
            int posY = row * TILE_SIZE;
            Block *NewBlock = nullptr;

            // Instancia objetos baseado no ID do tile
            switch (tileID)
            {
                case 0:
                    // bloco manageable
                    NewBlock = new Block(this, "Block" + std::to_string(managebleCounter), "../Assets/Sprites/Blocks/BlockJ.png", true, true);
                    managebleCounter = managebleCounter + 1;
                        break;
                case 1:
                    // Chão bordas
                    NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockBorder.png");
                    break;
                case 2:
                    // Chão interno
                    NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockInternal.png");
                        break;
                case 15:
                {
                    Dog *dog = new Dog(this, "Dog");
                    const Vector2 pos(posX, posY-1);
                    dog->SetPosition(pos);
                    break;
                }
                case 16:
                {
                    mCat = new Cat(this, "Player" + std::to_string(objNum));
                    const Vector2 pos(posX, posY-1);
                    mCat->SetPosition(pos);
                    break;
                }
                case 10:
                {
                    auto *spawner = new Spawner(this, "Spawner", SPAWN_DISTANCE);
                    const Vector2 pos(posX, posY);
                    spawner->SetPosition(pos);
                    break;
                }
                default:
                    break;
            }
            if (NewBlock)
            {
                const Vector2 pos(posX, posY);
                NewBlock->SetPosition(pos);
            }
            objNum++;
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

void Game::InitializeCore()
{
    mObjManager = new ObjectManager(this);
    mTerminal = new Terminal(mRenderer);
    mDialogManager = new DialogManager(mTerminal);
}

void Game::UnloadMenu()
{
    if (!mMainMenu)
        return;

    if (mAudio && mMainMenu->mMenuMusicPlaying)
    {
        mAudio->StopSound("MainMenu/Jazz.mp3");
        mMainMenu->mMenuMusicPlaying = false;
    }
    if (mAudio)
    {
        mAudio->PlaySound("MainMenu/Meow.mp3", false);
        mAudio->SetVolume("MainMenu/Meow.mp3", 10);
    }

    if (mAudio)
    {
        mAudio->PlaySound("Levels/BackgroundMusic.wav", true);
        mAudio->SetVolume("Levels/BackgroundMusic.wav", 48);
    }

    delete mMainMenu;
    mMainMenu = nullptr;
}

void Game::ProcessInput()
{
    if (mIsFading)
        return;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            Quit();
            break;
        }
        if (mMainMenu)
        {
            mMainMenu->HandleEvent(event);
            return;
        }

        mTerminal->ProcessEvent(event);
    }

    if (mCurrentScene == GameScene::MainMenu)
        return;

    if (mTerminal->IsActive())
        return;

    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::GoToNextScene()
{
    switch (mCurrentScene)
    {
        case GameScene::Level1:
        {
            SetScene(GameScene::Level2);
            break;
        }
        case GameScene::Level2:
        {
            SetScene(GameScene::Level3);
            break;
        }
        case GameScene::Level3:
        {
            SetScene(GameScene::Level4);
            break;
        }
        case GameScene::Level4:
        {
            SetScene(GameScene::Level5);
            break;
        }
        case GameScene::Level5:
        {
            SetScene(GameScene::MainMenu);
            break;
        }
        default:
            SetScene(GameScene::Level1);
            break;
    }
}

void Game::UpdateGame(float deltaTime)
{
    if (mIsFading)
    {
        Fade(deltaTime);

        // Update camera position
        UpdateCamera();
        return;
    }

    if (mAudio)
        mAudio->Update();

    if (mCurrentScene == GameScene::MainMenu)
        return;

    if (mLevelChangeCondition())
    {
        GoToNextScene();
        return;
    }

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();

    std::string command = mTerminal->ConsumeCommand();
    if (!command.empty())
    {
        SDL_Log("Terminal command: %s", command.c_str());
        ProcessTerminalCommand(command);
    }
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

    std::vector<Actor *> deadActors;
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
    if (!mCat)
        return;
    float desiredXLoc = mCat->GetPosition().x - WINDOW_WIDTH / 2.f;
    float desiredYLoc = mCat->GetPosition().y - WINDOW_HEIGHT/2.f;
    Vector2 clampedPos(desiredXLoc, desiredYLoc);

    SetCameraPos(clampedPos);
}

void Game::AddActor(Actor *actor)
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

void Game::RemoveActor(Actor *actor)
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

    std::sort(mDrawables.begin(), mDrawables.end(), [](DrawComponent *a, DrawComponent *b)
              { return a->GetDrawOrder() < b->GetDrawOrder(); });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent *collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent *collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    if (mCurrentScene == GameScene::MainMenu)
    {
        if (mMainMenu)
            mMainMenu->Draw(mIsDebugging);
    }
    else
    {
        for (auto drawable : mDrawables)
        {
            drawable->Draw(mRenderer);

            if (mIsDebugging)
            {
                for (auto comp : drawable->GetOwner()->GetComponents())
                {
                    comp->DebugDraw(mRenderer);
                }
            }
        }
        if (mTerminal)
            mTerminal->Draw();
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    // Delete level data
    if (mLevelData)
    {
        for (int i = 0; i < LEVEL_HEIGHT; ++i)
        {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    delete mObjManager;
    mObjManager = nullptr;

    if (mAudio)
    {
        mAudio->Shutdown();
        delete mAudio;
        mAudio = nullptr;
    }

    if (mMainMenu)
    {
        delete mMainMenu;
        mMainMenu = nullptr;
    }

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::ProcessTerminalCommand(const std::string &input)
{
    std::string command = input;
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    // Usa stringstream para dividir a linha em tokens (palavra por palavra)
    std::stringstream ss(command);
    std::string verb;             // O comando principal (ex: jump, set, get, add)
    std::string arg1, arg2, arg3; // Argumentos (ex: ACTOR_NAME, ATTRIBUTE, VALUE)

    ss >> verb; // Lê o primeiro token (o comando/verbo)

    if (verb == "jump")
    {
        mObjManager->Jump();
    }
    else if (verb == "get")
    {
        if (ss >> arg1)
        {
            std::string attributes = mObjManager->GetObjAttributes(arg1);
            mTerminal->AddLine(attributes);
        }
        else
        {
            mTerminal->AddLine("Usage: GET <Object Name>");
        }
    }
    else if (verb == "set")
    {
        if (ss >> arg1 >> arg2 >> arg3)
        {
            std::string originalValue = input.substr(input.find(arg3.substr(0, 1)));
            if (arg2 == "rotation")
            {
                if (!ValidateNumber(arg3))
                {
                    mTerminal->AddLine("Error: rotation must be a single number. Example: 45");
                    return;
                }
            }
            else if (arg2 == "scale" || arg2 == "position")
            {
                if (!ValidateVector2(arg3))
                {
                    mTerminal->AddLine("Error: value must be in format (x,y). Example: (1,2)");
                    return;
                }
            }
            else if (arg2 == "damage")
            {
                if (!ValidateNumber(arg3))
                {
                    mTerminal->AddLine("Error: damage must be a single number. Example: 0");
                    return;
                }
            }
            mObjManager->SetAttributeValue(arg1, arg2, arg3);
        }
        else
        {
            mTerminal->AddLine("Usage: set <Object Name> <Attribute> <Value>");
        }
    }
    else if (verb == "list")
    {
        std::vector<std::string_view> names = mObjManager->GetAllObjNames();
        std::string listStr = "Manageable Objects: ";
        for (const auto &name : names)
        {
            listStr += std::string(name) + " ";
        }
        mTerminal->AddLine(listStr);
    }
    else
    {
        mTerminal->AddLine("Unknown command: " + command);
    }
}

bool Game::ValidateNumber(const std::string &value)
{
    char *end;
    std::strtod(value.c_str(), &end);
    return end != value.c_str() && *end == '\0';
}

bool Game::ValidateVector2(const std::string &value)
{
    if (value.size() < 5 || value.front() != '(' || value.back() != ')')
        return false;

    std::string inside = value.substr(1, value.size() - 2);

    size_t comma = inside.find(',');
    if (comma == std::string::npos)
        return false;

    std::string x = inside.substr(0, comma);
    std::string y = inside.substr(comma + 1);

    auto isNumber = [](const std::string &s)
    {
        char *end;
        std::strtod(s.c_str(), &end);
        return end != s.c_str() && *end == '\0';
    };

    return isNumber(x) && isNumber(y);
}

void Game::RestartLevel()
{
    //SetScene(mCurrentScene);
}