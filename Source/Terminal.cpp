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
    : mRenderer(renderer), mFont(nullptr), mFontPath(fontPath), mPointSize(pointSize), mMaxLines(maxLines),
      mActive(true), mCursorBlink(0.0f), mCursorOn(true)
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
        else if (k == SDLK_UP)
        {
            if (!mHistory.empty())
            {
                if (mHistoryIndex == -1)
                    mHistoryIndex = (int)mHistory.size() - 1;
                else if (mHistoryIndex > 0)
                    mHistoryIndex--;

                mBuffer = mHistory[mHistoryIndex];
            }
        }
        else if (k == SDLK_DOWN)
        {
            if (!mHistory.empty() && mHistoryIndex != -1)
            {
                mHistoryIndex++;

                if (mHistoryIndex >= (int)mHistory.size())
                {
                    mHistoryIndex = -1;
                    mBuffer.clear();
                }
                else
                {
                    mBuffer = mHistory[mHistoryIndex];
                }
            }
        }
        else if (k == SDLK_TAB)
        {
            AppendChar("    ");
        }
        else if (k == SDLK_v && (SDL_GetModState() & KMOD_CTRL))
        {
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

    mBuffer.pop_back();
}

void Terminal::SubmitLine() {
    if (!mBuffer.empty()) {
        mLines.push_back(mBuffer);
        if (static_cast<int>(mLines.size()) > mMaxLines - 1) {
            mLines.pop_front();
        }
        mLastCommand = mBuffer;

        mHistory.push_back(mBuffer);
        mHistoryIndex = -1; // reset navegação

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

    const float levelHeightPx = ((Game::LEVEL_HEIGHT)*Game::TILE_SIZE);

    const float posX = Game::WINDOW_WIDTH / 2;
    const float width = static_cast<float>(Game::WINDOW_WIDTH);
    const float height = static_cast<float>(Game::WINDOW_HEIGHT) - levelHeightPx + Game::LEVEL_HEIGHT;
    const float posY = levelHeightPx + (height / 2.3);

    const float lineHeight = static_cast<float>(mPointSize + 4);
    const float padX = 8.0f;
    const float padY = 6.0f;

    mRenderer->DrawRect(Vector2(posX, posY), Vector2(width, height), 0.0f,
                        Vector3(0.03f, 0.03f, 0.03f), Vector2::Zero, RendererMode::TRIANGLES);

    float left = posX - width * 0.5f;
    float top = posY - height * 0.5f;

    float drawX = left + padX;
    float drawY = top + padY;
    float maxPixels = width - 2 * padX;

    for (const auto &ln : mLines)
    {
        Texture *tex = mFont->RenderText(ln, Vector3(1, 1, 1), mPointSize);
        if (!tex)
            continue;

        float texW = static_cast<float>(tex->GetWidth());
        float texH = static_cast<float>(tex->GetHeight());

        if (texW <= maxPixels)
        {
            Vector2 centerPos(drawX + texW * 0.5f, drawY + texH * 0.5f);
            mRenderer->DrawTexture(centerPos, Vector2(texW, texH), 0.0f,
                                   Vector3(1, 1, 1), tex, Vector4(0, 0, 1, 1),
                                   Vector2::Zero, false, 1.0f);
        }
        else
        {
            float u0 = (texW - maxPixels) / texW;
            Vector4 uv(u0, 0.0f, 1.0f, 1.0f);
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

    this->DrawHelper(left, top, width);

    const float blinkPeriod = 0.5f;
    static Uint32 lastTicks = SDL_GetTicks();
    Uint32 now = SDL_GetTicks();
    float dt = (now - lastTicks) / 1000.0f;
    if (dt >= blinkPeriod)
    {
        mCursorOn = !mCursorOn;
        lastTicks = now;
    }
}

void Terminal::AddLine(const std::string &line)
{
    const int MAX_CHARS = 80;

    // 1. Primeiro, quebrar por '\n'
    size_t start = 0;
    while (start <= line.size())
    {
        size_t pos = line.find('\n', start);
        std::string raw;

        if (pos == std::string::npos)
        {
            raw = line.substr(start);
            start = line.size() + 1;
        }
        else
        {
            raw = line.substr(start, pos - start);
            start = pos + 1;
        }

        // 2. Agora quebrar raw em linhas de no máximo MAX_CHARS
        size_t rstart = 0;
        while (rstart < raw.size())
        {
            size_t len = std::min((size_t)MAX_CHARS, raw.size() - rstart);
            std::string chunk = raw.substr(rstart, len);

            mLines.push_back(chunk);

            while ((int)mLines.size() > mMaxLines - 1)
                mLines.pop_front();

            rstart += MAX_CHARS;
        }
    }

    // 3. Garantir novamente que não ultrapassou
    while ((int)mLines.size() > mMaxLines - 1)
        mLines.pop_front();
}

void Terminal::DrawHelper(float left, float top, float width)
{
    {
        float panelWidth = 350.0f;
        float panelHeight = 240.0f;

        // canto direito do terminal
        float px = left + width - panelWidth - 10.0f;
        float py = top + 10.0f;

        // fundo do painel
        mRenderer->DrawRect(
            Vector2(px + panelWidth * 0.5f, py + panelHeight * 0.5f),
            Vector2(panelWidth, panelHeight),
            0.0f,
            Vector3(0.10f, 0.10f, 0.10f), // cor
            Vector2::Zero,
            RendererMode::TRIANGLES);

        const char *helpLines[] = {
            "Commands:",
            "  jump",
            "  get <objName>",
            "  set <objName> <attr> <value>",
            "  list",
            "Attributes (attr):",
            "  rotation (r)     value: x            (float)",
            "  position (p)    value: (x,y)        (floats)",
            "  scale (s)       value: (x,y)        (floats)"
        };

        float textX = px + 10.0f;
        float textY = py + 10.0f;

        for (const char *line : helpLines)
        {
            Texture *tex = mFont->RenderText(line, Vector3(1, 1, 1), mPointSize);
            if (tex)
            {
                float w = (float)tex->GetWidth();
                float h = (float)tex->GetHeight();

                mRenderer->DrawTexture(
                    Vector2(textX + w * 0.5f, textY + h * 0.5f),
                    Vector2(w, h),
                    0.0f,
                    Vector3(1, 1, 1),
                    tex,
                    Vector4(0, 0, 1, 1),
                    Vector2::Zero,
                    false,
                    1.0f);

                tex->Unload();
                delete tex;
                textY += h + 4.0f;
            }
        }
    }
}