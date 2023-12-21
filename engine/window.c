#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <cge/misc.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>
#include <math.h>

void GLAPIENTRY cgeGlErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	fprintf(stderr, "OPENGL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message);
}

void cgeGlfwErrorCallback(int error, const char* desc) {
	fprintf(stderr, "GLFW Error: %s\n", desc);
}

void cgeDestroyWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void cgeWindowPerspective(GLFWwindow* window, float fov, float nearClip, float farClip, mat4 persp) {
	int width, height;

	glfwGetWindowSize(window, &width, &height);

	glm_perspective(fov, (float) width / (float) height, nearClip, farClip, persp);
}

void cgeWindowOrtho(GLFWwindow* window, float scale, mat4 ortho) {
	int width, height;

	glfwGetWindowSize(window, &width, &height);

	glm_ortho(0.0f, ((float) width) / scale, 0.0f, ((float) height) / scale, -20.0f, 20.0f, ortho);	
}

void cgeWindowCamera(float posX, float posY, float posZ, float viewingAngle, mat4 cam) {
	glm_make_rad(&viewingAngle);

	float zDir = (float) sin((double) viewingAngle);
	float yDir = (float) cos((double) viewingAngle);

	vec3 eye = {posX, posY - yDir * posZ, posZ};
	vec3 dir = {0.0f, yDir, -zDir};
	vec3 up = {0.0f, zDir, yDir};

	glm_look(eye, dir, up, cam);
}

GLFWwindow* cgeCreateWindow(char* title) {
	if(!glfwInit()) {
		cgeCrash("GLFW could not be initialized.");
	}

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

#if(BUILD == CGE_DEBUG)
	glfwSetErrorCallback(cgeGlfwErrorCallback);
#endif

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, title, glfwGetPrimaryMonitor(), NULL);
	if(!window) {
		cgeCrash("GLFW Window creation failed.");
	} else {
		printf("GLFW Window created successfully.\n");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwShowWindow(window);

	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK) {
		cgeCrash("GLEW could not be initialized!");
	}

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(cgeGlErrorCallback, 0);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);


	return window;
}
