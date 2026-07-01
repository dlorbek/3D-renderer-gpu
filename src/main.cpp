#define SDL_MAIN_HANDLED

#include "headers/game.h"
#include "headers/engine.h"
#include <filesystem>



#define SCREEN_W 1920
#define SCREEN_H 1080

const int window_w = 800;
const int window_h = window_w;


int main(int argc, char** argv){
    std::string filepath;
    std::vector<openstl::Triangle> triangles;
    if(argc > 1){
        filepath = argv[1];
        if(argc > 2){
            for(int i = 2; i < argc; i++){
                filepath += " ";
                filepath += argv[i];
            }
        }

        printf("Opening file: %s\n", filepath.data());
        std::ifstream file(filepath.data(), std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file '" << filepath << "'" << std::endl;
            return 1;
        }

        // Deserialize the triangles in either binary or ASCII format
        triangles = openstl::deserializeStl(file);
        file.close();

        printf("STL loaded. Triangle count: %d\n", triangles.size());
        std::string filename = std::filesystem::path(filepath).filename().string();
        
    } else {
        printf("No file provided.\n");
    }

    
    
    
    

    // create window and engine
    std::string winName = "Dan's 3D Viewer";
    Game game;
    game.createWindow(winName.data(), window_w, window_h, SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    Engine engine(window_w, window_h);
    engine.initOpenGL(window_w, window_h);
    

    // build model from triangles
    if(argc > 1){
        engine.models.emplace_back(engine.createModel(triangles));
    }
    
    

    const Uint64 targetFrameTime = 1000.0 / game.FPS;
    static int frames = 0;
    static Uint64 fpsTimer = SDL_GetTicks();

    if(argc == 1) printf("\n --- PRESS O TO OPEN A MODEL ---\n\n");

    //---------------------------------GAME LOOP-----------------------------------------------
    float angle = 0.0f;
    while(game.gameState != GameState::EXIT){
        Uint64 frameStart = SDL_GetTicks();
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            game.handleEvents(&e);
            engine.handleEvents(&e, game.window);
        }

        
        
        engine.render(game.window);
        
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK) engine.clickDuration++;


        // FPS Measurement
        frames++;
        if (SDL_GetTicks() - fpsTimer >= 1000) {
            printf("\rFPS: %d", frames);
            frames = 0;
            fpsTimer = SDL_GetTicks();
        }

        // 60 FPS cap
        Uint64 frameTime = SDL_GetTicks() - frameStart;
        if(frameTime < targetFrameTime) SDL_Delay(targetFrameTime - frameTime);

    }
    //---------------------------------GAME LOOP-----------------------------------------------

    
    return 0;
} // end main




