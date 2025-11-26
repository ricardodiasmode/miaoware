//
// Created by Hector on 25/11/2025.
//
#include "Terminal.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/Font.h"
#include "./Renderer/Texture.h"
#include "./Game.h" // para Game::WINDOW_WIDTH / HEIGHT
#include <SDL_ttf.h>

Terminal::Terminal(Renderer *renderer, const std::string &fontPath, int pointSize, int maxLines)
    : mRenderer(renderer), mFont(nullptr), mFontPath(fontPath), mPointSize(pointSize), mMaxLines(maxLines), mActive(false), mCursorBlink(0.0f), mCursorOn(true)
{
    // Cria e carrega a Font (aloca dinamicamente para compatibilidade)
    mFont = new Font();
    if (!mFont->Load(mFontPath))
    {
        SDL_Log("Terminal: falha ao carregar fonte %s", mFontPath.c_str());
        delete mFont;
        mFont = nullptr;
    }

    // inicia histórico com uma linha com prompt (opcional)
    mLines.clear();
    mBuffer.clear();
    mLastCommand.clear();
}

Terminal::~Terminal()
{
    if (mFont)
    {
        mFont->Unload();
        delete mFont;
        mFont = nullptr;
    }
}

void Terminal::Toggle()
{
    SetActive(!mActive);
}

void Terminal::SetActive(bool active)
{
    if (active == mActive)
        return;
    mActive = active;

    if (mActive)
    {
        // ativa captura de texto do SDL
        SDL_StartTextInput();
    }
    else
    {
        SDL_StopTextInput();
    }

    // reset cursor blink
    mCursorBlink = 0.0f;
    mCursorOn = true;
}

void Terminal::ProcessEvent(const SDL_Event &ev)
{
    if (!mActive)
        return;

    // Usamos SDL_TEXTINPUT para pegar texto (UTF-8) e SDL_KEYDOWN para backspace / enter / ctrl combos
    if (ev.type == SDL_TEXTINPUT)
    {
        // ev.text.text é uma string UTF-8 (C-string)
        AppendChar(std::string(ev.text.text));
    }
    else if (ev.type == SDL_KEYDOWN)
    {
        SDL_Keycode k = ev.key.keysym.sym;

        if (k == SDLK_BACKSPACE)
        {
            Backspace();
        }
        else if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
        {
            SubmitLine();
        }
        else if (k == SDLK_TAB)
        {
            // opcional: inserir 4 espaços
            AppendChar(std::string("    "));
        }
        else if (k == SDLK_v && (SDL_GetModState() & KMOD_CTRL))
        {
            // Ctrl+V -> colar (do clipboard)
            char *clip = SDL_GetClipboardText();
            if (clip)
            {
                AppendChar(std::string(clip));
                SDL_free(clip);
            }
        }
    }
}

void Terminal::AppendChar(const std::string &utf8)
{
    // limita tamanho do buffer (p/ evitar crescimento infinito)
    const size_t MAXBUF = 512;
    if (mBuffer.size() + utf8.size() > MAXBUF)
        return;
    mBuffer += utf8;
}

void Terminal::Backspace()
{
    if (mBuffer.empty())
        return;
    // remove último byte UTF-8 corretamente: simplificação — remove 1 byte até encontrar início (bom o suficiente para ascii)
    // Para suporte UTF-8 robusto, você pode usar uma lib. Aqui assumimos ascii/utf8 simples.
    mBuffer.pop_back();
}

void Terminal::SubmitLine()
{
    // adicionar linha ao histórico
    if (!mBuffer.empty())
    {
        mLines.push_back(mBuffer);
        if (static_cast<int>(mLines.size()) > mMaxLines - 1)
        {
            mLines.pop_front();
        }
        mLastCommand = mBuffer;
        mBuffer.clear();
    }
}

std::string Terminal::ConsumeCommand()
{
    std::string tmp = mLastCommand;
    mLastCommand.clear();
    return tmp;
}

void Terminal::Draw()
{
    if (!mFont)
        return;

    // cálculo real baseado no level (posX,posY estão no centro do terminal)
    const float levelHeightPx = ((Game::LEVEL_HEIGHT)*Game::TILE_SIZE);

    const float posX = Game::WINDOW_WIDTH / 2; // seu terminal já usa isso como centro
    const float width = static_cast<float>(Game::WINDOW_WIDTH);
    const float height = static_cast<float>(Game::WINDOW_HEIGHT) - levelHeightPx + Game::LEVEL_HEIGHT;
    const float posY = levelHeightPx + (height / 2.3); // mantém sua posição atual do terminal

    const float lineHeight = static_cast<float>(mPointSize + 4);
    const float padX = 8.0f;
    const float padY = 6.0f;

    // desenha fundo (centro, size)
    mRenderer->DrawRect(Vector2(posX, posY), Vector2(width, height), 0.0f,
                        Vector3(0.03f, 0.03f, 0.03f), Vector2::Zero, RendererMode::TRIANGLES);

    // calcular canto superior-esquerdo (pois posX,posY são centro do terminal)
    float left = posX - width * 0.5f;
    float top = posY - height * 0.5f; // **top** (superior)

    // início do desenho do texto: canto superior esquerdo + padding
    // parâmetros já definidos antes:
    float drawX = left + padX;          // canto esquerdo + padding
    float drawY = top + padY;           // canto superior + padding
    float maxPixels = width - 2 * padX; // área útil horizontal

    // --- desenhar linhas do histórico (cada linha) ---
    for (const auto &ln : mLines)
    {
        Texture *tex = mFont->RenderText(ln, Vector3(1, 1, 1), mPointSize);
        if (!tex)
            continue;

        float texW = static_cast<float>(tex->GetWidth());
        float texH = static_cast<float>(tex->GetHeight());

        if (texW <= maxPixels)
        {
            // cabe totalmente: desenhar normalmente
            Vector2 centerPos(drawX + texW * 0.5f, drawY + texH * 0.5f); // converter top-left->center
            mRenderer->DrawTexture(centerPos, Vector2(texW, texH), 0.0f,
                                   Vector3(1, 1, 1), tex, Vector4(0, 0, 1, 1),
                                   Vector2::Zero, false, 1.0f);
        }
        else
        {
            // maior que área: mostrar APENAS A PARTE DIREITA (últimos maxPixels)
            float u0 = (texW - maxPixels) / texW; // proporção inicial em UV para mostrar só fim
            Vector4 uv(u0, 0.0f, 1.0f, 1.0f);     // (u0,v0,u1,v1)
            Vector2 size(maxPixels, texH);
            Vector2 centerPos(drawX + maxPixels * 0.5f, drawY + texH * 0.5f);
            mRenderer->DrawTexture(centerPos, size, 0.0f,
                                   Vector3(1, 1, 1), tex, uv, Vector2::Zero, false, 1.0f);
        }

        tex->Unload();
        delete tex;

        drawY += lineHeight;
        if (drawY + lineHeight > top + height - padY)
            break;
    }

    // --- desenhar prompt (mesma lógica, mas importante pro cursor) ---
    std::string prompt = "mioware@user:~$ " + mBuffer + (mCursorOn ? "_" : " ");
    Texture *ptex = mFont->RenderText(prompt, Vector3(1, 1, 1), mPointSize);
    if (ptex)
    {
        float texW = static_cast<float>(ptex->GetWidth());
        float texH = static_cast<float>(ptex->GetHeight());

        if (texW <= maxPixels)
        {
            Vector2 centerPos(drawX + texW * 0.5f, drawY + texH * 0.5f);
            mRenderer->DrawTexture(centerPos, Vector2(texW, texH), 0.0f,
                                   Vector3(1, 1, 1), ptex, Vector4(0, 0, 1, 1),
                                   Vector2::Zero, false, 1.0f);
        }
        else
        {
            // cortar e mostrar a parte DIREITA (o final da string) para que o cursor fique visível
            float u0 = (texW - maxPixels) / texW;
            Vector4 uv(u0, 0.0f, 1.0f, 1.0f);
            Vector2 size(maxPixels, texH);
            Vector2 centerPos(drawX + maxPixels * 0.5f, drawY + texH * 0.5f);
            mRenderer->DrawTexture(centerPos, size, 0.0f,
                                   Vector3(1, 1, 1), ptex, uv, Vector2::Zero, false, 1.0f);
        }

        ptex->Unload();
        delete ptex;
    }

    // atualizar piscar de cursor (você já tinha isso; mantive)
    const float blinkPeriod = 0.5f; // segundos
    static Uint32 lastTicks = SDL_GetTicks();
    Uint32 now = SDL_GetTicks();
    float dt = (now - lastTicks) / 1000.0f;
    if (dt >= blinkPeriod)
    {
        mCursorOn = !mCursorOn;
        lastTicks = now;
    }
}

// ...
void Terminal::AddLine(const std::string &line)
{
    mLines.push_back(line);
    if (static_cast<int>(mLines.size()) > mMaxLines - 1)
    {
        mLines.pop_front();
    }
}