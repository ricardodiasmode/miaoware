#pragma once
#include <SDL.h>
#include <string>

class Game;
class Renderer;
class Font;

class MainMenu {
public:
    MainMenu(Game* game, Font* font);

    void HandleEvent(const SDL_Event& ev);
    void Draw(bool debug);

private:
    void StartGame();
    bool IsInStartRect(int x, int y) const;
    bool IsInQuitRect(int x, int y) const;

    Game* mGame;
    Font* mFont;
    int mSelected = 0; // 0 = Start, 1 = Exit

    const float mMinX = 446.0f;
    const float mMaxX = 558.0f;
    const float mMinY = 543.0f;
    const float mMaxY = 626.0f;

    const float mQuitMinX = 622.0f;
    const float mQuitMaxX = 718.0f;
    const float mQuitMinY = 541.0f;
    const float mQuitMaxY = 624.0f;
};
