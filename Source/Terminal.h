//
// Created by Hector on 25/11/2025.
//
#pragma once
#include <string>
#include <deque>
#include <SDL.h>
#include <vector>

class Renderer;
class Font;
class Texture;

class Terminal
{
public:
  Terminal(Renderer *renderer,
           const std::string &fontPath = "../Assets/Fonts/Arial.ttf",
           int pointSize = 16,
           int maxLines = 12);

  ~Terminal();

  void Toggle();
  void SetActive(bool active);
  bool IsActive() const { return mActive; }
  void AddLine(const std::string &line);

  void ProcessEvent(const SDL_Event &ev);

  void Draw();

  std::string ConsumeCommand();

private:
  void AppendChar(const std::string &utf8);
  void Backspace();
  void SubmitLine();
  void DrawHelper(float left, float top, float width);
  std::vector<std::string> WrapText(const std::string& text, float maxPixels);

  Renderer *mRenderer;
  Font *mFont;
  std::string mFontPath;
  int mPointSize;
  int mMaxLines;

  bool mActive;
  std::deque<std::string> mLines;
  std::string mBuffer;
  std::string mLastCommand;

  float mCursorBlink;
  bool mCursorOn;

  std::vector<std::string> mHistory;
  int mHistoryIndex = -1;
};
