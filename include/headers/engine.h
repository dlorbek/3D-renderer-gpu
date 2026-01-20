#ifndef ENGINE_H_
#define ENGINE_H_


#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include "headers/stl.h"
#include "headers/math3d.h"
#include "glad/glad.h"
#include "headers/shaderClass.h"


struct Vertex
{
    Vec3 pos;
    Vec3 normal;
};



struct Triangle
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 n;
    float avgZ;
};





class Engine{
    public:
        Engine(int, int);
        ~Engine();
        void initOpenGL(int, int);
        void buildMesh(std::vector<Triangle>& triangles);
        Vec3 convert(const openstl::Vec3& v);
        Triangle convert(const openstl::Triangle& tri);
        void parseTriangles(std::vector<openstl::Triangle>& libTri, std::vector<Triangle>& Tri);

        void render(SDL_Window*);
        void handleEvents(SDL_Event* event);

        
    private:
        int w;
        int h;
        bool rotating, translating;
        bool wireframe;
        float scale;
        float FOV;
        Mat4 modelMatrix, view, proj;
        Vec3 lightDir;
        
        GLuint VBO, VAO, EBO, nVBO;
        std::unique_ptr<Shader> shaderProgram;
        Vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};

        std::vector<Vertex> vertices;


};


#endif