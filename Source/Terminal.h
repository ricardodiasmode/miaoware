//
// Created by Hector on 25/11/2025.
//
#pragma once
#include <string>
#include <deque>
#include <SDL.h>

class Renderer;
class Font;
class Texture;

class Terminal {
public:
  Terminal(Renderer* renderer,
           const std::string& fontPath = "../Assets/Fonts/Arial.ttf",
           int pointSize = 18,
           int maxLines = 10);

  ~Terminal();

  // liga/desliga o terminal (toggle)
  void Toggle();
  void SetActive(bool active);
  bool IsActive() const { return mActive; }
  void AddLine(const std::string& line);

  // processa eventos SDL (use antes de encaminhar eventos ao jogo)
  void ProcessEvent(const SDL_Event& ev);

  // desenha por cima (chamar depois de tudo em GenerateOutput)
  void Draw();

  // retorna e limpa o último comando enviado (ENTER)
  std::string ConsumeCommand();

private:
  void AppendChar(const std::string& utf8);
  void Backspace();
  void SubmitLine();

  Renderer* mRenderer;
  Font* mFont;                 // ponteiro para Font carregada
  std::string mFontPath;
  int mPointSize;
  int mMaxLines;

  bool mActive;
  std::deque<std::string> mLines; // histórico visível (últimas mMaxLines - 1 são history)
  std::string mBuffer;            // linha atual sendo digitada
  std::string mLastCommand;       // comando pronto após ENTER

  // cursor
  float mCursorBlink;    // contador para piscar cursor
  bool mCursorOn;
};
