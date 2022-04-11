#include <iostream>
#include "GLFW/glfw3.h"
#include "glad.h"
#include "TextRenderer.h"

const unsigned short int SCR_WIDTH = 800, SCR_HEIGHT = 600;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game", NULL, NULL);
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

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	TextRenderer textRenderer("D:/OddStoneGames/FPSGame/FPSGame/FPSGame/src/Content/Fonts/HelpMe.ttf", 48, glm::vec2(SCR_WIDTH, SCR_HEIGHT));

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(.15, .15, .15, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		textRenderer.RenderText("Play!", glm::vec2(25.0f, 125.0f), 1.0f, glm::vec3(0, 1, 0));
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}