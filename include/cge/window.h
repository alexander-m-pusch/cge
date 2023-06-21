#ifndef CGE_WINDOW_H
#define CGE_WINDOW_H

#include <GLFW/glfw3.h>
#include <cglm/cam.h>

void cgeDestroyWindow(GLFWwindow* window);
void cgeWindowPerspective(GLFWwindow* window, float fov, float nearClip, float farClip, mat4 persp);
void cgeWindowOrtho(GLFWwindow* window, float scale, mat4 ortho);
void cgeWindowCamera(float posX, float posY, float posZ, float viewingAngle, mat4 cam);
GLFWwindow* cgeCreateWindow(char* title);

#endif
