#ifndef GAME_H_
#define GAME_H_


#include <stdio.h>
#include <vector>
#include <SDL3/SDL.h>




enum class GameState {PLAY, EXIT};

class Game {
    public:
        Game();
        ~Game();
        SDL_Texture* loadTexture(const char* path);
        SDL_Window* window;
        SDL_Renderer* renderer;
        void handleEvents(SDL_Event* event);
        const int FPS = 60;
        GameState gameState;
        void render();
        void createWindow(const char* title, int w, int h, Uint32 flags);
        
    private:
        
        
        int _screenWidth;
        int _screenHeight;
        bool dragging;
        int dx, dy; 
        

        
};

#endif