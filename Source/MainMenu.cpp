#include "Game.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Renderer/Font.h"
#include <SDL.h>
#include <string>

class MainMenu {
public:
    MainMenu(Game* game, Font* font)
        : mGame(game), mFont(font), mSelected(0), mClosed(false) {}

    void Close() { mClosed = true; }

    void HandleKeyPress(const SDL_Event& ev)
    {
        if (ev.type != SDL_KEYDOWN) return;
        SDL_Keycode k = ev.key.keysym.sym;
        if (k == SDLK_UP) {
            mSelected = (mSelected + 2 - 1) % 2; // wrap up
        } else if (k == SDLK_DOWN) {
            mSelected = (mSelected + 1) % 2; // wrap down
        } else if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
            OnClick();
        }
    }

    void Draw()
    {
        Renderer* r = mGame->GetRenderer();
        r->DrawRect(
            Vector2(Game::WINDOW_WIDTH * 0.5f, Game::WINDOW_HEIGHT * 0.5f),
            Vector2((float)Game::WINDOW_WIDTH, (float)Game::WINDOW_HEIGHT),
            0.0f,
            Vector3(0.0f, 0.0f, 0.0f),
            Vector2::Zero,
            RendererMode::TRIANGLES);
        // Center positions
        float cx = Game::WINDOW_WIDTH * 0.5f;
        float cy = Game::WINDOW_HEIGHT * 0.5f;

        // Logo text
        if (mFont) {
            Texture* logo = mFont->RenderText("miaoware", Vector3(1,1,1), 36);
            if (logo) {
                float w = (float)logo->GetWidth();
                float h = (float)logo->GetHeight();
                r->DrawTexture(Vector2(cx, cy - 120), Vector2(w, h), 0.0f, Vector3(1,1,1), logo,
                               Vector4(0,0,1,1), Vector2::Zero, false, 1.0f);
                logo->Unload();
                delete logo;
            }
        }

        // Buttons
        DrawButton(Vector2(cx, cy), "Compartilhar", mSelected == 0);
        DrawButton(Vector2(cx, cy + 80), "Quit", mSelected == 1);
    }

    bool IsClosed() const { return mClosed; }

private:
    void DrawButton(const Vector2& center, const std::string& label, bool selected)
    {
        Renderer* r = mGame->GetRenderer();
        Vector2 size(280.0f, 48.0f);
        Vector3 bg = selected ? Vector3(0.2f, 0.4f, 0.9f) : Vector3(0.1f, 0.25f, 0.7f);
        r->DrawRect(center, size, 0.0f, bg, Vector2::Zero, RendererMode::TRIANGLES);

        if (mFont) {
            Texture* txt = mFont->RenderText(label.c_str(), Vector3(1,1,1), 22);
            if (txt) {
                float w = (float)txt->GetWidth();
                float h = (float)txt->GetHeight();
                r->DrawTexture(center, Vector2(w, h), 0.0f, Vector3(1,1,1), txt,
                               Vector4(0,0,1,1), Vector2::Zero, false, 1.0f);
                txt->Unload();
                delete txt;
            }
        }
    }

    void OnClick()
    {
        if (mSelected == 0) {
            // Compartilhar
            mGame->SetScene(Game::GameScene::Playing);
            Close();
        } else {
            // Quit
            mGame->Quit();
            Close();
        }
    }

    Game* mGame;
    Font* mFont;
    int mSelected;
    bool mClosed;
};
