#define SDL_MAIN_HANDLED

#include "headers/game.h"
#include "headers/engine.h"




#define SCREEN_W 1920
#define SCREEN_H 1080




int main(int argc, char** argv){
    
    int window_w = 600;
    int window_h = window_w;
    
    Engine engine(window_w, window_h);
    

    std::string filename;
    if(argc > 1){
        filename = argv[1];
        if(argc > 2){
            for(int i = 2; i < argc; i++){
                filename += " ";
                filename += argv[i];
            }
        }
        
    } else {
        printf("No file,\n");
        return 1;
    }

    printf("Opening file: %s", filename.data());
    std::ifstream file(filename.data(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "'" << std::endl;
        return 1;
    }

    // Deserialize the triangles in either binary or ASCII format
    std::vector<openstl::Triangle> triangles = openstl::deserializeStl(file);
    file.close();

    printf("STL loaded. Triangle count: %d\n", triangles.size());

    // convert OpenSTL triangles to my triangles
    std::vector<Triangle> modelTriangles;
    engine.parseTriangles(triangles, modelTriangles);



    engine.buildMesh(modelTriangles);



    Game game;
    game.createWindow("Dan's 3D Viewer", window_w, window_h, SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    engine.initOpenGL(window_w, window_h);
    


    const Uint64 targetFrameTime = 1000.0 / game.FPS;
    static int frames = 0;
    static Uint64 fpsTimer = SDL_GetTicks();

    
    //---------------------------------GAME LOOP-----------------------------------------------
    float angle = 0.0f;
    while(game.gameState != GameState::EXIT){
        Uint64 frameStart = SDL_GetTicks();
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            game.handleEvents(&e);
            engine.handleEvents(&e);
        }

        
        
        engine.render(game.window);
    


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




