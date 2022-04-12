#include <iostream>
#include "GLFW/glfw3.h"
#include "glad.h"
#include "TextRenderer.h"

unsigned short int SCR_WIDTH = 1920, SCR_HEIGHT = 1080;

bool isGamePaused = true;

void MouseScrolledCallback(GLFWwindow* window, double xoffset, double yoffset)
{

}

void MouseButtonPressedCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void KeyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (isGamePaused)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}
}

void WindowCloseCallback(GLFWwindow* window)
{

}

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game", glfwGetPrimaryMonitor(), NULL);
	glfwMakeContextCurrent(window);

	int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladStatus)
	{ 
		std::cout << "Failed To Initialize Glad" << std::endl;
		glfwTerminate();
		return -1;
	}

	// OpenGL state
	// ------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetScrollCallback(window, MouseScrolledCallback);
	glfwSetMouseButtonCallback(window, MouseButtonPressedCallback);
	glfwSetWindowSizeCallback(window, WindowResizeCallback);
	glfwSetWindowCloseCallback(window, WindowCloseCallback);
	glfwSetKeyCallback(window, KeyPressedCallback);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	TextRenderer textRenderer("D:/OddStoneGames/FPSGame/FPSGame/FPSGame/src/Content/Fonts/HelpMe.ttf", 48, glm::vec2(SCR_WIDTH, SCR_HEIGHT));

	isGamePaused = true;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(.15, .15, .15, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		textRenderer.RenderText("Play!", glm::vec2(25.0f, 225.0f), 1.0f, glm::vec3(0, 1, 0));
		textRenderer.RenderText("Options", glm::vec2(25.0f, 125.0f), 1.0f, glm::vec3(1, .7, 0));
		textRenderer.RenderText("Quit!", glm::vec2(25.0f, 25.0f), 1.0f, glm::vec3(1, 0, 0));

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}