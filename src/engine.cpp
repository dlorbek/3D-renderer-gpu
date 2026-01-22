#include "headers/engine.h"

#include <math.h>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>



Engine::Engine(int w_, int h_){
    printf("Engine created\n");
    w = w_;
    h = h_;
    FOV = 45.0f;
    rotating = false;
    wireframe = false;
    translating = false;
    scale = 1.0f;
    lightDir = normalize(Vec3{0.0f, 1.0f, -1.0f});
    
}

Engine::~Engine(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shaderProgram->Delete();
    printf("Engine destroyed\n");
}

void Engine::initOpenGL(int w, int h){
    
    glViewport(0, 0, w, h);

    shaderProgram = std::make_unique<Shader>("data/Shaders/github.vert", "data/Shaders/github.frag");
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &nVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);


    view = Mat4::translation({0.0f, 0.0f, -2.0f});
    
    

}

void Engine::render(SDL_Window* window){
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->Activate();

    proj = Mat4::projection(radians(FOV), (float)w/h, 0.1f, 100.0f);

    int modelLoc = glGetUniformLocation(shaderProgram->ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix.m[0]);
    
    int viewLoc = glGetUniformLocation(shaderProgram->ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0]);

    int projLoc = glGetUniformLocation(shaderProgram->ID, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj.m[0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    SDL_GL_SwapWindow(window);
}






Vec3 Engine::convert(const openstl::Vec3& v){
    return { v.x, v.y, v.z };
}

Triangle Engine::convert(const openstl::Triangle& t){
    return { convert(t.v0), convert(t.v1), convert(t.v2), convert(t.normal) };
}

void Engine::buildMesh(std::vector<Triangle>& triangles)
{
    vertices.clear();
    vertices.reserve(triangles.size() * 3);

    for (const Triangle& t : triangles)
    {
        Vertex v0;
        v0.pos    = t.v0;
        v0.normal = t.n;

        Vertex v1;
        v1.pos    = t.v1;
        v1.normal = t.n;

        Vertex v2;
        v2.pos    = t.v2;
        v2.normal = t.n;

        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    triangles.clear();
    triangles.shrink_to_fit();
}


void Engine::parseTriangles(std::vector<openstl::Triangle>& libTri, std::vector<Triangle>& tri){
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













void Engine::handleEvents(SDL_Event* event){
    switch (event->type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if(event->button.button == SDL_BUTTON_LEFT){
            rotating = true;
        } else if(event->button.button == SDL_BUTTON_RIGHT){
            translating = true;
        } 
        
        
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if(event->button.button == SDL_BUTTON_LEFT){
            rotating = false;
        } else if (event->button.button == SDL_BUTTON_RIGHT){
            translating = false;
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (rotating){
            float angle_x = event->motion.yrel * 0.01;
            float angle_y = event->motion.xrel * 0.01;
            
            modelMatrix = Mat4::rotationX(angle_x) * Mat4::rotationY(angle_y) * modelMatrix;
        } else if (translating){
            float trans_y = event->motion.yrel * 0.005;
            float trans_x = event->motion.xrel * 0.005;

            modelMatrix = Mat4::translation({trans_x, -trans_y, 0.0f}) * modelMatrix;
        }
        break;

    case SDL_EVENT_KEY_UP:
        switch (event->key.key)
        {
            case SDLK_UP:
                FOV += 1.0f;
                break;
            
            case SDLK_DOWN:
                if(FOV > 0.2f) FOV -= 1.0f;
                break;

            case SDLK_W:
                lightDir.y += 0.1f;
                lightDir = normalize(lightDir);
                break;

            case SDLK_S:
                lightDir.y -= 0.1f;
                lightDir = normalize(lightDir);
                break;

            case SDLK_A:
                lightDir.x -= 0.1f;
                lightDir = normalize(lightDir);
                break;

            case SDLK_D:
                lightDir.x += 0.1f;
                lightDir = normalize(lightDir);
                break;
            
            default:
                break;
        }
        
        break;

    case SDL_EVENT_MOUSE_WHEEL:
    {
        float scale = 1.0f + event->wheel.y * 0.1f;
        modelMatrix = Mat4::scale({scale, scale, scale}) * modelMatrix;
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

