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
#include "Actors/Mushroom.h"
#include "Actors/SpawnBlock.h"
#include "Utils/DialogManager.h"
#include "Utils/ObjectManager.h"
#include "Renderer/Font.h"
#include "AudioSystem.h"

Game::Game()
    : mWindow(nullptr)
    , mRenderer(nullptr)
    , mTicksCount(0)
    , mIsRunning(true)
    , mIsDebugging(false)
    , mUpdatingActors(false)
    , mCameraPos(0.f, 0.f)
    , mCat(nullptr)
    , mLevelData(nullptr)
    , mTerminal(nullptr)
    , mCurrentScene(GameScene::MainMenu)
    , mUiFont(nullptr)
    , mAudio(nullptr)
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

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mUiFont = new Font();
    mUiFont->Load("../Assets/Fonts/Arial.ttf");

    mAudio = new AudioSystem();
    if (!mAudio->Initialize())
    {
        SDL_Log("AudioSystem failed to initialize");
    }

    mTicksCount = SDL_GetTicks();

    mObjManager = new ObjectManager(this);
    mTerminal = new Terminal(mRenderer);
    mDialogManager = new DialogManager(mTerminal);

    return true;
}

void Game::InitializeActors()
{
    int** level = LoadLevel("../Assets/Levels/Level1-1/level1-1.csv", Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
    //int **level = LoadLevel("../Assets/Levels/Level1-3/level1-3.csv", Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
    BuildLevel(level, Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
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
    auto *bg = new Background(this, "Background", "../Assets/Sprites/Background.png");
    bg->SetPosition(Vector2(3408, 210));

    // Percorre a matriz de tiles
    int objNum = 0;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            int tileID = levelData[row][col];

            int posX = col * TILE_SIZE;
            int posY = row * TILE_SIZE;
            Block *NewBlock = nullptr;

            // Instancia objetos baseado no ID do tile
            switch (tileID)
            {
            case 0:
                // Chão
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockA.png");
                break;
            case 1:
            {
                // Interrogação
                // Hardcodando posições porque nao foi especificado como seria a pré-definição (poderia ser em .csv também)
                static const std::vector<Vector2> spawnableBlocks = {
                    Vector2(2, 34),
                    Vector2(10, 162)};
                const Vector2 currentPos(row, col);
                if (std::find(spawnableBlocks.begin(), spawnableBlocks.end(), currentPos) != spawnableBlocks.end())
                {
                    NewBlock = SpawnBlock::create<Mushroom>(
                        this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockC.png", 100.f);
                }
                else
                {
                    NewBlock = new MovingBlock(
                        this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockC.png");
                }
                break;
            }
            case 4:
                // Brick
                NewBlock = new MovingBlock(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockB.png");
                break;
            case 8:
                // Chão especial
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockD.png");
                break;
            case 10:
            {
                auto *spawner = new Spawner(this, "Spawner", SPAWN_DISTANCE);
                const Vector2 pos(posX, posY);
                spawner->SetPosition(pos);
                break;
            }
            case 12:
                // Cano verde cima direita
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockG.png");
                break;
            case 2:
                // Cano verde cima esquerda
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockF.png");
                break;
            case 9:
                // cano verde baixo esquerda
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockH.png");
                break;
            case 6:
                // Cano verde baixo direita
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockI.png");
                break;
            case 7:
                // Cano verde topo direito
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockD.png");
                break;
            case 16:
            {
                mCat = new Cat(this, "Player" + std::to_string(objNum));
                const Vector2 pos(posX, posY);
                mCat->SetPosition(pos);
            }
            case 99:
                // bloco manageable
                NewBlock = new Block(this, "Block" + std::to_string(objNum), "../Assets/Sprites/Blocks/BlockC.png");
                break;
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
        // no menu principal, apenas processa eventos normais
        // para evitar de "abrir o terminal"
        if (mCurrentScene != GameScene::MainMenu)
        {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                mTerminal->Toggle();
                continue;
            }

            // Terminal captura eventos so na gameplay
            mTerminal->ProcessEvent(event);
        }
    }

    if (mCurrentScene == GameScene::MainMenu)
    {
        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_RETURN])
        {
            if (mAudio) mAudio->PlaySound("MainMenuMeow.mp3", false);
            InitializeActors();
            mCurrentScene = GameScene::Playing;
        }
        return;
    }

    if (mTerminal->IsActive())
        return;

    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame(float deltaTime)
{
    if (mCurrentScene == GameScene::MainMenu)
    {
        if (mAudio) mAudio->Update();
        return; // não atualiza nada no menu principal
    }
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();

    if (mAudio) mAudio->Update();

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

    if (desiredXLoc < 0.f)
        desiredXLoc = 0.f;

    Vector2 clampedPos(desiredXLoc, GetCameraPos().y);

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
        if (mUiFont)
        {
            Texture *tex = mUiFont->RenderText("Compartilhar", Vector3(1,1,1), 28);
            if (tex)
            {
                float w = (float)tex->GetWidth();
                float h = (float)tex->GetHeight();
                Vector2 center(Game::WINDOW_WIDTH * 0.5f, Game::WINDOW_HEIGHT * 0.5f);
                mRenderer->DrawTexture(center, Vector2(w, h), 0.0f,
                                       Vector3(1,1,1), tex, Vector4(0,0,1,1), Vector2::Zero, false, 1.0f);
                tex->Unload();
                delete tex;
            }
        }
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
        if (ss >> arg1 >> arg2 >> arg3) {
            std::string originalValue = input.substr(input.find(arg3.substr(0, 1)));
            if (arg2 == "rotation")
            {
                if (!ValidateRotation(arg3))
                {
                    mTerminal->AddLine("Error: rotation must be a single number. Example: 45");
                    return;
                }
            }
            else if (arg2 == "scale" || arg2 == "position")
            {
                if (!ValidateVector2(arg3))
                {
                    mTerminal->AddLine("Error: value must be in format (x,y). Example: (1.0, 2.5)");
                    return;
                }
            }
            mObjManager->SetAttributeValue(arg1, arg2, arg3);
        } else  {
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

bool Game::ValidateRotation(const std::string& value)
{
    char* end;
    std::strtod(value.c_str(), &end);
    return end != value.c_str() && *end == '\0';
}

bool Game::ValidateVector2(const std::string& value)
{
    if (value.size() < 5 || value.front() != '(' || value.back() != ')')
        return false;

    std::string inside = value.substr(1, value.size() - 2);

    size_t comma = inside.find(',');
    if (comma == std::string::npos)
        return false;

    std::string x = inside.substr(0, comma);
    std::string y = inside.substr(comma + 1);

    auto isNumber = [](const std::string& s)
    {
        char* end;
        std::strtod(s.c_str(), &end);
        return end != s.c_str() && *end == '\0';
    };

    return isNumber(x) && isNumber(y);
}
