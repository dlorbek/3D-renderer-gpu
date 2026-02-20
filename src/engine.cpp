#include "headers/engine.h"

#include <math.h>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "headers/fileManager.h"



Engine::Engine(int w_, int h_){
    
    w = w_;
    h = h_;
    FOV = 45.0f;
    rotating = false;
    translating = false;
    looking = false;
    scale = 1.0f;
    clickDuration = 0;
    lightDir = normalize(Vec3{0.0f, 1.0f, -1.0f});
    models.reserve(5);
    modelColors = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f},
        {0.0f, 0.5f, 0.5f}
    };
    outlineColor = framebuffer.hoverColor;
    printf("Engine created\n");
    
}

Engine::~Engine(){
    shaderProgram->Delete();
    pickerProgram->Delete();
    outlineProgram->Delete();
    printf("\nEngine destroyed\n");
}

void Engine::initOpenGL(int w, int h){
    
    glViewport(0, 0, w, h);

    shaderProgram = std::make_unique<Shader>("data/Shaders/default.vert", "data/Shaders/default.frag");
    shaderProgram->uModel = glGetUniformLocation(shaderProgram->ID, "model");
    shaderProgram->uView = glGetUniformLocation(shaderProgram->ID, "view");
    shaderProgram->uProj = glGetUniformLocation(shaderProgram->ID, "proj");

    pickerProgram = std::make_unique<Shader>("data/Shaders/picker.vert", "data/Shaders/picker.frag");
    pickerProgram->uModel = glGetUniformLocation(pickerProgram->ID, "model");
    pickerProgram->uView = glGetUniformLocation(pickerProgram->ID, "view");
    pickerProgram->uProj = glGetUniformLocation(pickerProgram->ID, "proj");

    outlineProgram = std::make_unique<Shader>("data/Shaders/outline.vert", "data/Shaders/outline.frag");
    outlineProgram->uModel = glGetUniformLocation(outlineProgram->ID, "model");
    outlineProgram->uView = glGetUniformLocation(outlineProgram->ID, "view");
    outlineProgram->uProj = glGetUniformLocation(outlineProgram->ID, "proj");
    

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);



    view = Mat4::translation({0.0f, 0.0f, -2.0f});

    // Create off-screen frame buffer
    glGenFramebuffers(1, &framebuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

    // Color attachment
    glGenTextures(1, &framebuffer.texture);
    glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture, 0);

    // Depth buffer
    glGenRenderbuffers(1, &framebuffer.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer initialisation success.\n");
    } else {
        printf("Framebuffer initialisation failure.\n");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    framebuffer.colorIDloc = glGetUniformLocation(pickerProgram->ID, "colorID");
    framebuffer.highlightColorLoc = glGetUniformLocation(outlineProgram->ID, "highlightColor");

}

void Engine::renderFramebuffer(SDL_Window* window){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);
    glViewport(0, 0, w, h);

    GLuint clearValue = 0;
    glClearBufferuiv(GL_COLOR, 0, &clearValue);
    glClear(GL_DEPTH_BUFFER_BIT);

    pickerProgram->Activate();

    glUniformMatrix4fv(pickerProgram->uProj, 1, GL_FALSE, &proj.m[0]);
    glUniformMatrix4fv(pickerProgram->uView, 1, GL_FALSE, &view.m[0]);

    for(const Model& model : models){
        glUniformMatrix4fv(pickerProgram->uModel, 1, GL_FALSE, &model.transformation.m[0]);
        glUniform1ui(framebuffer.colorIDloc, model.id);

        glBindVertexArray(model.VAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.vertices.size());
    }

}


void Engine::render(SDL_Window* window)
{
    proj = Mat4::projection(radians(FOV), (float)w/h, 0.1f, 100.0f);
    view = camera.getView();

    // ---------- PICK BUFFER ----------
    renderFramebuffer(window);

    // ---------- NORMAL SCENE ----------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);

    glEnable(GL_STENCIL_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    shaderProgram->Activate();

    glUniformMatrix4fv(shaderProgram->uProj, 1, GL_FALSE, &proj.m[0]);
    glUniformMatrix4fv(shaderProgram->uView, 1, GL_FALSE, &view.m[0]);

    for (const Model& model : models)
    {
        glUniformMatrix4fv(shaderProgram->uModel, 1, GL_FALSE, &model.transformation.m[0]);

        if (model.isHovered || model.isSelected)
            glStencilMask(0xFF);   
        else
            glStencilMask(0x00);   

        glBindVertexArray(model.VAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.vertices.size());
    }

    // ---------- OUTLINE PASS ----------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    outlineProgram->Activate();

    glUniformMatrix4fv(outlineProgram->uProj, 1, GL_FALSE, &proj.m[0]);
    glUniformMatrix4fv(outlineProgram->uView, 1, GL_FALSE, &view.m[0]);
    

    for (const Model& model : models)
    {
        if (model.isHovered) {
            glUniform3fv(framebuffer.highlightColorLoc, 1, &outlineColor.x);
        } else if (model.isSelected){
            glUniform3fv(framebuffer.highlightColorLoc, 1, &framebuffer.selectColor.x);
        } else {
            continue;
        }

        Mat4 outlineModel = model.transformation * Mat4::scale({1.03f, 1.03f, 1.03f});

        glUniformMatrix4fv(outlineProgram->uModel, 1, GL_FALSE, &outlineModel.m[0]);

        glBindVertexArray(model.VAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.vertices.size());
    }

    // ---------- RESET STATE ----------
    glEnable(GL_DEPTH_TEST);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    SDL_GL_SwapWindow(window);
}




void Engine::parseTriangles(std::vector<openstl::Triangle>& libTri, std::vector<Triangle>& tri){
    tri.reserve(tri.size() + libTri.size());

    for (auto& t : libTri) {

        Triangle out;

        out.v0 = {
            t.v0.x,
            t.v0.y,
            t.v0.z
        };
        out.v1 = {
            t.v1.x,
            t.v1.y,
            t.v1.z
        };
        out.v2 = {
            t.v2.x,
            t.v2.y,
            t.v2.z
        };

        Vec3 a = out.v1 - out.v0;
        Vec3 b = out.v2 - out.v0;
        out.n = normalize(a.cross(b));

        tri.push_back(out);
    }

    // clear library triangles to free memory
    libTri.clear();
    libTri.shrink_to_fit();
}

void Engine::parseTrianglesResize(std::vector<openstl::Triangle>& libTri, std::vector<Triangle>& tri){
    tri.reserve(tri.size() + libTri.size());

     // Step 1: compute centroid and max coordinate in one pass
    Vec3 center = {0,0,0};
    float maxCoord = 0.0f;

    for (const auto& t : libTri) {
        // accumulate center
        center.x += t.v0.x + t.v1.x + t.v2.x;
        center.y += t.v0.y + t.v1.y + t.v2.y;
        center.z += t.v0.z + t.v1.z + t.v2.z;

        // find max coordinate for scaling
        maxCoord = std::max({maxCoord,
            std::abs(t.v0.x), std::abs(t.v0.y), std::abs(t.v0.z),
            std::abs(t.v1.x), std::abs(t.v1.y), std::abs(t.v1.z),
            std::abs(t.v2.x), std::abs(t.v2.y), std::abs(t.v2.z)});
    }

    center = center * (1.0f / (libTri.size() * 3)); // average

    float scale = 0.8f / maxCoord;

    // Step 2: convert and normalize in a second pass
    for (auto& t : libTri) {

        Triangle out;
        //out.n = { t.normal.x, t.normal.y, t.normal.z };

        out.v0 = {
            (t.v0.x - center.x) * scale,
            (t.v0.y - center.y) * scale,
            (t.v0.z - center.z) * scale
        };
        out.v1 = {
            (t.v1.x - center.x) * scale,
            (t.v1.y - center.y) * scale,
            (t.v1.z - center.z) * scale
        };
        out.v2 = {
            (t.v2.x - center.x) * scale,
            (t.v2.y - center.y) * scale,
            (t.v2.z - center.z) * scale
        };

        Vec3 a = out.v1 - out.v0;
        Vec3 b = out.v2 - out.v0;
        out.n = normalize(a.cross(b));

        tri.push_back(out);
    }

    // clear library triangles to free memory
    libTri.clear();
    libTri.shrink_to_fit();


}


Model Engine::createModel(std::vector<openstl::Triangle>& inputTri){
    static uint64_t id = 1;
    static uint8_t colorIDX = 2;
    std::vector<Triangle> triangles;
    parseTrianglesResize(inputTri, triangles);

    // load triangles into Model
    Model newModel;
    newModel.vertices.clear();
    newModel.vertices.reserve(triangles.size() * 3);

    for (const Triangle& t : triangles)
    {
        Vertex v0;
        v0.pos    = t.v0;
        v0.normal = t.n;
        v0.color = modelColors[colorIDX];

        Vertex v1;
        v1.pos    = t.v1;
        v1.normal = t.n;
        v1.color = modelColors[colorIDX];

        Vertex v2;
        v2.pos    = t.v2;
        v2.normal = t.n;
        v2.color = modelColors[colorIDX];

        newModel.vertices.push_back(v0);
        newModel.vertices.push_back(v1);
        newModel.vertices.push_back(v2);
    }

    

    // initialise OpenGL objects for Model
    glGenVertexArrays(1, &newModel.VAO);
    glGenBuffers(1, &newModel.VBO);
    
    glBindVertexArray(newModel.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, newModel.VBO);
    glBufferData(GL_ARRAY_BUFFER, newModel.vertices.size() * sizeof(Vertex), newModel.vertices.data(), GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Colors
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    newModel.isSelected = false;
    newModel.isHovered = false;
    newModel.id = id++;
    colorIDX++;
    if(colorIDX >= modelColors.size()) colorIDX = 0;


    return newModel;
}


bool Engine::deleteModel(uint64_t id){
    for(size_t i = 0; i < models.size(); i++){
        if(models[i].id == id){
            models.erase(models.begin() + i);
            return true;
        }
    }

    return false;
}



uint64_t Engine::hoveringOver(int x, int y){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);
    uint32_t pixel;
    glReadPixels(x, h - y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);
    return pixel;
}


void Engine::handleEvents(SDL_Event* event, SDL_Window* window){
    switch (event->type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if(event->button.button == SDL_BUTTON_LEFT){
            rotating = true;
            outlineColor = framebuffer.pressColor;

            
        } else if(event->button.button == SDL_BUTTON_RIGHT){
            translating = true;
        } 

        
        
        
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if(event->button.button == SDL_BUTTON_LEFT){
            rotating = false;
            outlineColor = framebuffer.hoverColor;
            if(clickDuration < 8){
                for(Model& model : models){
                    if(model.isSelected){
                        model.isSelected = false;
                        continue;
                    }
                    if(model.isHovered) model.isSelected = true;
                }
            }
            clickDuration = 0;


        } else if (event->button.button == SDL_BUTTON_RIGHT){
            translating = false;
        }

        
        
        
        

        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (rotating){
            float angle_x = event->motion.yrel * 0.01;
            float angle_y = event->motion.xrel * 0.01;

            for(Model& model : models){
                if(model.isHovered || model.isSelected){
                    model.transformation = Mat4::rotationX(angle_x) * Mat4::rotationY(angle_y) * model.transformation;
                }
            }
            
        } else if (translating){
            float trans_y = event->motion.yrel * 0.005;
            float trans_x = event->motion.xrel * 0.005;

            for(Model& model : models){
                if(model.isHovered || model.isSelected){
                    model.transformation = Mat4::translation({trans_x, -trans_y, 0.0f}) * model.transformation;
                }
            }
        } else if (looking) {
            camera.pitch += event->motion.yrel * camera.sensitivity;
            camera.yaw -= event->motion.xrel * camera.sensitivity;
            if(camera.pitch > 89.0f) camera.pitch =  89.0f;
            if(camera.pitch < -89.0f) camera.pitch = -89.0f;
            
            camera.updateDirection();
        } else {
            uint64_t id = hoveringOver(event->button.x, event->button.y);
            for(Model& model : models){
                model.isHovered = (id == model.id);
            }
            
        }
        break;
    
    case SDL_EVENT_MOUSE_WHEEL:
    {
        float scale = 1.0f + event->wheel.y * 0.1f;
        int hCount = 0;
        for(Model& model : models){
            if(model.isHovered || model.isSelected){
                model.transformation = Mat4::scale({scale, scale, scale}) * model.transformation;
                hCount++;
            }
        }
        if(hCount == 0){
            camera.position = camera.position + (camera.direction * event->wheel.y);
        }
    }
        break;

    case SDL_EVENT_KEY_DOWN:
        switch(event->key.key)
        {
            case SDLK_W:
                camera.position = camera.position + (camera.direction * camera.speed);
                break;
            case SDLK_S:
                camera.position = camera.position - (camera.direction * camera.speed);
                break;
            case SDLK_A:
                {
                Vec3 right = normalize(camera.direction.cross(Vec3{0,1,0}));
                camera.position = camera.position - (right * camera.speed);
                break;
                }
            case SDLK_D:
                {
                Vec3 right = normalize(camera.direction.cross(Vec3{0,1,0}));
                camera.position = camera.position + (right * camera.speed);
                break;
                }
            default:
                break;
        }
        break;

    case SDL_EVENT_KEY_UP:
        switch (event->key.key)
        {
            case SDLK_L:
                looking = !looking;
                SDL_SetWindowRelativeMouseMode(window, looking);
                break;

            case SDLK_I:
                printf("\n Models: %d\n", models.size());
                break;

            case SDLK_DELETE:
            {
                for(Model& model : models){
                    if(model.isSelected) deleteModel(model.id);
                }    
            }
                break;
            case SDLK_O:
            {
                std::string filepath = browseFiles();
                printf("Opening file: %s\n", filepath.data());
                std::ifstream file(filepath.data(), std::ios::binary);
                if (!file.is_open()) {
                    std::cerr << "Error: Unable to open file '" << filepath << "'" << std::endl;
                    break;
                }

                // Deserialize the triangles in either binary or ASCII format
                std::vector<openstl::Triangle> triangles = openstl::deserializeStl(file);
                file.close();

                printf("STL loaded. Triangle count: %d\n", triangles.size());
                models.emplace_back(createModel(triangles));
                printf("Model created.\n");
            }
                break;
            
            default:
                break;
        }
        
        break;

    

    case SDL_EVENT_WINDOW_RESIZED:
        w = event->window.data1;
        h = event->window.data2;
        glViewport(0, 0, w, h);
        break;
    
    default:
        
        break;
    }
}

