#include "headers/game.h"





Game::Game(){
    gameState = GameState::PLAY;
    SDL_Init(SDL_INIT_VIDEO);
    dragging = false;
    printf("Game created.\n");
    
};

Game::~Game(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("RIP window\n");
};

void Game::createWindow(const char* title, int w, int h, Uint32 flags){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(title, w, h, flags);
    renderer = SDL_CreateRenderer(window, NULL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


    SDL_SetWindowRelativeMouseMode(window, false);
    printf("Window opened.\n");
}





void Game::handleEvents(SDL_Event* event){
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        gameState = GameState::EXIT;
        break;

    case SDL_EVENT_KEY_UP:
        // printf("\n%s key pressed.\n", SDL_GetKeyName(event->key.key));
        switch (event->key.key){
            case SDLK_ESCAPE:
                gameState = GameState::EXIT;
                break;
            default:
                break;
        }
        break;
    
    default:
        
        break;
    }
}


void Game::render(){
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderClear(renderer);
}








