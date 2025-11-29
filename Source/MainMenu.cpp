#include "MainMenu.h"
#include "Game.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Renderer/Font.h"
#include "AudioSystem.h"

MainMenu::MainMenu(Game* game, Font* font)
    : mGame(game), mFont(font), mSelected(0)
{
}

void MainMenu::HandleEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_KEYDOWN)
    {
        const SDL_Keycode k = ev.key.keysym.sym;
        if (k == SDLK_LEFT)
        {
            mSelected = (mSelected + 2 - 1) % 2; // wrap to 1 when at 0
        }
        else if (k == SDLK_RIGHT)
        {
            mSelected = (mSelected + 1) % 2;
        }
        else if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
        {
            if (mSelected == 0) StartGame(); else mGame->Quit();
        }
        else if (k == SDLK_q)
        {
            mGame->Quit();
        }
    }
    else if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT)
    {
        int mx = ev.button.x;
        int my = ev.button.y;
        if (IsInStartRect(mx, my))
        {
            StartGame();
        }
        else if (IsInQuitRect(mx, my))
        {
            mGame->Quit();
        }
    }
}

bool MainMenu::IsInStartRect(int x, int y) const
{
    return x >= mMinX && x <= mMaxX && y >= mMinY && y <= mMaxY;
}

bool MainMenu::IsInQuitRect(int x, int y) const
{
    return x >= mQuitMinX && x <= mQuitMaxX && y >= mQuitMinY && y <= mQuitMaxY;
}

void MainMenu::Draw(bool debug)
{
    Renderer* r = mGame->GetRenderer();

    if (Texture* bg = r->GetTexture("../Assets/Sprites/Menu/Background.jpg"))
    {
        Vector2 center(Game::WINDOW_WIDTH * 0.5f, Game::WINDOW_HEIGHT * 0.5f);
        Vector2 size((float)Game::WINDOW_WIDTH, (float)Game::WINDOW_HEIGHT);
        r->DrawTexture(center, size, 0.0f, Vector3(1,1,1), bg, Vector4(0,0,1,1), Vector2::Zero, false, 1.0f);
    }

    if (mFont)
    {
        Texture *tex = mFont->RenderText("Compartilhar", Vector3(1,1,1), 28);
        if (tex)
        {
            float w = (float)tex->GetWidth();
            float h = (float)tex->GetHeight();
            Vector2 center(Game::WINDOW_WIDTH * 0.5f, Game::WINDOW_HEIGHT * 0.5f);
            r->DrawTexture(center, Vector2(w, h), 0.0f,
                           Vector3(1,1,1), tex, Vector4(0,0,1,1), Vector2::Zero, false, 1.0f);
            tex->Unload();
            delete tex;
        }
    }

    const float thickness = 3.0f;
    const Vector3 white(1.0f, 1.0f, 1.0f);
    const Vector3 red(0.9f, 0.2f, 0.2f);

    const Vector3 startColor = (mSelected == 0) ? red : white;
    r->DrawRect(Vector2((mMinX+mMaxX)*0.5f, mMinY), Vector2(mMaxX-mMinX, thickness), 0.0f, startColor, Vector2::Zero, RendererMode::TRIANGLES);
    r->DrawRect(Vector2((mMinX+mMaxX)*0.5f, mMaxY), Vector2(mMaxX-mMinX, thickness), 0.0f, startColor, Vector2::Zero, RendererMode::TRIANGLES);
    r->DrawRect(Vector2(mMinX, (mMinY+mMaxY)*0.5f), Vector2(thickness, mMaxY-mMinY), 0.0f, startColor, Vector2::Zero, RendererMode::TRIANGLES);
    r->DrawRect(Vector2(mMaxX, (mMinY+mMaxY)*0.5f), Vector2(thickness, mMaxY-mMinY), 0.0f, startColor, Vector2::Zero, RendererMode::TRIANGLES);

    const Vector3 quitColor = (mSelected == 1) ? red : white;
    r->DrawRect(Vector2((mQuitMinX+mQuitMaxX)*0.5f, mQuitMinY), Vector2(mQuitMaxX-mQuitMinX, thickness), 0.0f, quitColor, Vector2::Zero, RendererMode::TRIANGLES);
    r->DrawRect(Vector2((mQuitMinX+mQuitMaxX)*0.5f, mQuitMaxY), Vector2(mQuitMaxX-mQuitMinX, thickness), 0.0f, quitColor, Vector2::Zero, RendererMode::TRIANGLES);
    r->DrawRect(Vector2(mQuitMinX, (mQuitMinY+mQuitMaxY)*0.5f), Vector2(thickness, mQuitMaxY-mQuitMinY), 0.0f, quitColor, Vector2::Zero, RendererMode::TRIANGLES);
    r->DrawRect(Vector2(mQuitMaxX, (mQuitMinY+mQuitMaxY)*0.5f), Vector2(thickness, mQuitMaxY-mQuitMinY), 0.0f, quitColor, Vector2::Zero, RendererMode::TRIANGLES);

    if (debug)
    {
        int mx = 0, my = 0;
        Uint32 buttons = SDL_GetMouseState(&mx, &my);
        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            r->DrawRect(Vector2((float)mx, (float)my), Vector2(8.0f, 8.0f), 0.0f,
                        Vector3(1.0f, 0.2f, 0.2f), Vector2::Zero, RendererMode::TRIANGLES);

            if (mFont)
            {
                std::string coords = "(" + std::to_string(mx) + ", " + std::to_string(my) + ")";
                Texture* t = mFont->RenderText(coords.c_str(), Vector3(1,1,1), 18);
                if (t)
                {
                    float tw = (float)t->GetWidth();
                    float th = (float)t->GetHeight();
                    Vector2 pos((float)mx + 14.0f, (float)my - th - 6.0f);
                    r->DrawTexture(pos, Vector2(tw, th), 0.0f, Vector3(1,1,1), t,
                                   Vector4(0,0,1,1), Vector2::Zero, false, 1.0f);
                    t->Unload();
                    delete t;
                }
            }
        }
    }
}

void MainMenu::StartGame()
{
    if (mGame->mAudio)
    {
        mGame->mAudio->PlaySound("MainMenuMeow.mp3", false);
    }
    mGame->InitializeActors();
    mGame->mCurrentScene = Game::GameScene::Playing;
}
