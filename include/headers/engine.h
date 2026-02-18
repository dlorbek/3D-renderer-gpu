#ifndef ENGINE_H_
#define ENGINE_H_


#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include "headers/stl.h"
#include "headers/math3d.h"
#include "glad/glad.h"
#include "headers/shaderClass.h"
#include "headers/camera.h"


struct Vertex
{
    Vec3 pos;
    Vec3 normal;
    Vec3 color;
};



struct Triangle
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 n;
};


struct Model
{
    GLuint VBO = 0;
    GLuint VAO = 0;
    std::vector<Vertex> vertices;
    Mat4 transformation{};
    bool isSelected = false, isHovered = false;
    uint64_t id = 0;

    Model() = default;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    // ✅ Move constructor
    Model(Model&& other) noexcept
        : VBO(other.VBO),
          VAO(other.VAO),
          vertices(std::move(other.vertices)),
          transformation(other.transformation),
          isSelected(other.isSelected),
          isHovered(other.isHovered),
          id(other.id)
    {
        other.VAO = 0;
        other.VBO = 0;
    }

    // ✅ Move assignment
    Model& operator=(Model&& other) noexcept
    {
        if (this != &other)
        {
            cleanup();

            VBO = other.VBO;
            VAO = other.VAO;
            vertices = std::move(other.vertices);
            transformation = other.transformation;
            isSelected = other.isSelected;
            isHovered = other.isHovered;
            id = other.id;

            other.VAO = 0;
            other.VBO = 0;
        }
        return *this;
    }

    ~Model()
    {
        cleanup();
        printf("Model %llu destroyed\n", id);
    }

private:
    void cleanup()
    {
        if (VBO) glDeleteBuffers(1, &VBO);
        if (VAO) glDeleteVertexArrays(1, &VAO);
        VBO = VAO = 0;
    }
};




struct Framebuffer
{
    GLuint FBO, texture, depth;
    GLuint colorIDloc;
};





class Engine{
    public:
        Engine(int, int);
        ~Engine();
        void initOpenGL(int, int);
        void parseTriangles(std::vector<openstl::Triangle>& libTri, std::vector<Triangle>& Tri);
        void parseTrianglesResize(std::vector<openstl::Triangle>& libTri, std::vector<Triangle>& tri);
        Model createModel(std::vector<openstl::Triangle>& inputTri);
        bool deleteModel(uint64_t id);
        uint64_t hoveringOver(int x, int y);
        

        void render(SDL_Window*);
        void renderFramebuffer(SDL_Window*);
        void handleEvents(SDL_Event* event, SDL_Window* window);

        std::vector<Model> models;
        
    private:
        int w;
        int h;
        bool rotating, translating, looking;
        float scale;
        float FOV;
        Mat4 view, proj;
        Vec3 lightDir;

        std::unique_ptr<Shader> shaderProgram;
        std::unique_ptr<Shader> pickerProgram;
        std::unique_ptr<Shader> outlineProgram;

        Camera camera;
        Framebuffer framebuffer;
        
        


};


#endif