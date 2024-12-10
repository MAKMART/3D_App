#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Model/Model.h"

class Application {
public:
    Application(const char* title, int width, int height);
    ~Application();
    void Run();

private:
    GLFWwindow* window;
    int width, height;
    const char* title;
    glm::vec4 backgroundColor;
    
    bool isFullscreen = false;
    bool WIREFRAME_MODE{};
    int windowedWidth, windowedHeight;
    int windowedPosX, windowedPosY;

    Camera camera;
    bool firstMouse = true;
    float lastX, lastY;
    glm::vec3 objectOffset;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    Model* model;
    Model* model2;

    Shader* shader;

    void initWindow();
    void handleFullscreenToggle(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void processInput();
};
