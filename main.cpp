#include <iostream>
#include <GLAD\glad.h>
#include <GLFW\glfw3.h>
#include <glm\matrix.hpp>
#include <glm\vec3.hpp>

int main()	
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Window", NULL, NULL);

	float currentTime{}, lastTime{}, deltaTime{};
	if (window == nullptr)
	{
		std::cout << "Failed to initialize GLFW!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!\n";
		return -1;
	}

	glViewport(0, 0, 1920, 1080);
	while (!glfwWindowShouldClose(window))
	{
		currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastTime;
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glfwSwapBuffers(window);
		lastTime = currentTime;
		glfwPollEvents();
	}
	glfwTerminate();
}