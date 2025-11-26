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

  void Toggle();
  void SetActive(bool active);
  bool IsActive() const { return mActive; }
  void AddLine(const std::string& line);

  void ProcessEvent(const SDL_Event& ev);

  void Draw();

  std::string ConsumeCommand();

private:
  void AppendChar(const std::string& utf8);
  void Backspace();
  void SubmitLine();

  Renderer* mRenderer;
  Font* mFont;
  std::string mFontPath;
  int mPointSize;
  int mMaxLines;

  bool mActive;
  std::deque<std::string> mLines;
  std::string mBuffer;
  std::string mLastCommand;


  float mCursorBlink;
  bool mCursorOn;
};
