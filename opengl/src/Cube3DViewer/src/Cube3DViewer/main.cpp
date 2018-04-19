#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include "GLUtil.h"
#if defined(USE_NETWORK_SOCKET)
#include "SocketPoseService.h"
#else
#include "FakePoseGenerator.h"
#endif

float pos[3] = {0.0, 0.0, 0.0};
float rot[3] = {0.0, 0.0, 0.0};

#if defined(USE_NETWORK_SOCKET)
SocketPoseService sps;
#else
FakePoseGenerator fpg;
#endif

void initializeGL()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	mat4x4 view;
	vec3 eye = {0.0f, 0.0f, 0.0f};
	vec3 center = {0.0f, 0.0f, 1.0f};
	vec3 up = {0.0f, 1.0f, 0.0f};
	mat4x4_look_at(view, eye, center, up);
	glLoadMatrixf((const GLfloat*)view);
}

void resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity();            // Reset The Projection Matrix
	mat4x4 projection;
	float aspect = width / float(height);
	mat4x4_perspective(projection, 45.0f, aspect, 0.1f, 100.0f);
	glLoadMatrixf((const GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);
}

void paintGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glPushMatrix();

	// update pose
#if defined(USE_NETWORK_SOCKET)
	sps.getPose(pos, rot);
#else
	fpg.getPose(pos, rot);
#endif

	glTranslated(pos[0], pos[1], pos[2]);
	glRotated(rot[1], 0.0, 1.0, 0.0);
	glRotated(rot[0], 1.0, 0.0, 0.0);
	glRotated(rot[2], 0.0, 0.0, 1.0);
	//printf("%f, %f, %f, %f, %f, %f\n", pos[0], pos[1], pos[2], rot[0], rot[1], rot[2]);

	//glLineWidth(2.5);
	GLUtil::drawAxis(5.0);
	GLUtil::drawCube();

	glPopMatrix();
}

void mainLoop(GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		paintGL();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        printf("space\n");
}

int main(int argc, const char *argv[])
{
	// Initialize GLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Select OpenGL 2.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	GLFWwindow *window = glfwCreateWindow(640, 480, "Cube 3D Viewer", NULL, NULL);
	if (!window) {
		printf("Failed to create GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Move window to (100, 100)
	glfwSetWindowPos(window, 100, 100);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, keyCallback);

	// Initialize GLFW
	initializeGL();
	resizeGL(640, 480);

	// Enter the main loop
	mainLoop(window);

	// Close OpenGL window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
