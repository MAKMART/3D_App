#include "Application.h"

Application::Application(const char* title, int width, int height)
    : title(title), width(width), height(height), camera(glm::vec3(0.0f, 0.0f, 3.0f)), /*backgroundColor(0.1f, 0.1f, 0.1f, 1.0f)*/backgroundColor(1.0f, 1.0f, 1.0f, 1.0f), objectOffset(0.0f, 0.0f, -2.0f) {
    initWindow();

    // Load shaders and model
    shader = new Shader("shaders/Vertex.vert", "shaders/Fragment.frag");

    model = new Model("assets/models/nanosuit/nanosuit.obj");
    model2 = new Model("assets/models/plane/untitled.obj");

    // Set initial mouse position
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    glfwSetWindowUserPointer(window, this);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing
    glEnable(GL_CULL_FACE);    // Enable face culling
    glCullFace(GL_BACK);
}

Application::~Application() {
    delete shader;
    delete model;
    glfwTerminate();
}

void Application::initWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
}

void Application::Run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        /*if (camera.Position.y < 0.0f)
        {
            camera.Position.y = 0.0f;
        }*/
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (WIREFRAME_MODE)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, camera.NEAR_PLANE, camera.FAR_PLANE);
        glm::mat4 view = camera.GetViewMatrix();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        // Render first model (e.g., nanosuit)
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        // Position the nanosuit at the camera's position
        modelMatrix = glm::translate(modelMatrix, camera.Position);

        // Offset the nanosuit relative to the camera in the camera's local space
        //glm::vec3 nanosuitOffset(0.5f, -0.5f, -1.0f); // Adjust this for positioning
        glm::vec3 nanosuitOffset(0.0f, 0.0f, -2.0f);
        modelMatrix = glm::translate(modelMatrix,
            camera.Front * nanosuitOffset.z +
            camera.Up * nanosuitOffset.y +
            camera.Right * nanosuitOffset.x);

        // Align the nanosuit's rotation with the camera's orientation
        modelMatrix *= glm::mat4(glm::mat3(camera.GetViewMatrix())); // Extract rotation

        // Optionally scale the nanosuit to fit the scene
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));

        // Pass the nanosuit model matrix to the shader
        shader->setMat4("model", modelMatrix);

        // Draw the nanosuit
        model->Draw(*shader);

        // Render second model (e.g., plane)
        glm::mat4 modelMatrix2 = glm::mat4(1.0f); // Identity matrix
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.0f, -1.0f, -5.0f)); // Different position
        modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(1.0f)); // Different scale
        shader->setMat4("model", modelMatrix2);
        model2->Draw(*shader);

        //model->Draw(*shader);
        //model2->Draw(*shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    handleFullscreenToggle(window);
    float speed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
        //objectOffset.z -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        //objectOffset.z += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
        //objectOffset.x += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
        //objectOffset.x -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        WIREFRAME_MODE = !WIREFRAME_MODE;
}

void Application::handleFullscreenToggle(GLFWwindow* window) {
    static bool fullscreenTogglePressed = false;

    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !fullscreenTogglePressed) {
        fullscreenTogglePressed = true;

        if (isFullscreen) {
            glfwSetWindowMonitor(window, nullptr, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
            isFullscreen = false;
        }
        else {
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
            glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            isFullscreen = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
        fullscreenTogglePressed = false;
    }
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (application) {
        if (application->firstMouse) {
            application->lastX = static_cast<float>(xpos);
            application->lastY = static_cast<float>(ypos);
            application->firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - application->lastX);
        float yoffset = static_cast<float>(application->lastY - ypos);
        application->lastX = static_cast<float>(xpos);
        application->lastY = static_cast<float>(ypos);

        application->camera.ProcessMouseMovement(xoffset, yoffset, true);
    }
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    static Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
