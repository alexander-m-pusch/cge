#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cge/window.h>
#include <cge/shader.h>
#include <cge/renderbatch.h>
#include <cge/texture.h>
#include <cge/spritesheet.h>
#include <cge/3drenderer.h>
#include <cge/misc.h>

static int esc_pushed = 0;
static int w_pushed = 0;
static int a_pushed = 0;
static int s_pushed = 0;
static int d_pushed = 0;
static int f5_pushed = 0;

static unsigned int gedrueckt = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		esc_pushed = 1;
	}
	if(key == GLFW_KEY_W && action == GLFW_PRESS) {
		w_pushed = 1;
	}
	if(key == GLFW_KEY_A && action == GLFW_PRESS) {
                a_pushed = 1;
        }
	if(key == GLFW_KEY_S && action == GLFW_PRESS) {
                s_pushed = 1;
        }
	if(key == GLFW_KEY_D && action == GLFW_PRESS) {
                d_pushed = 1;
        }
	if(key == GLFW_KEY_F5 && action == GLFW_PRESS && (!gedrueckt)) {
		f5_pushed = 1;
		gedrueckt = 1;
	}
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                esc_pushed = 1;
        }
        if(key == GLFW_KEY_W && action == GLFW_RELEASE) {
                w_pushed = 0;
        }
        if(key == GLFW_KEY_A && action == GLFW_RELEASE) {
                a_pushed = 0;
        }
        if(key == GLFW_KEY_S && action == GLFW_RELEASE) {
                s_pushed = 0;
        }
        if(key == GLFW_KEY_D && action == GLFW_RELEASE) {
                d_pushed = 0;
        }
	if(key == GLFW_KEY_F5 && action == GLFW_RELEASE) {
		gedrueckt = 0;
        }

}

int main(void) {
	GLFWwindow* window = cgeCreateWindow("Deine Mudda");
	
	glfwSetKeyCallback(window, key_callback);

	struct cgeShader* shader = cgeCreateShader();	
	int vs = cgeAttachShaderFromFile(shader, "build/vertex.glsl", GL_VERTEX_SHADER);
	int fs = cgeAttachShaderFromFile(shader, "build/fragment.glsl", GL_FRAGMENT_SHADER);

#if(BUILD == CGE_DEBUG)
	printf("This is a DEBUG build. Expect your log to get spammed and general weirdness, this is not release.\n");
#endif

	printf("Using CGE Version %s\n", cgeVersion());

	if((vs + fs) != 0) {
		cgeCrash("Could not load shaders.\n");
	}

	int ss = cgeMakeShader(shader);

	if(ss != 0) {
		cgeCrash("Shader compilation failed.\n");
	}

	printf("Shader successfully compiled.\n");

	mat4 cameraMatrix;
	mat4 perspMatrix;

	cgeWindowCamera(0.0f, 0.0f, 5.0f, 90.0f, cameraMatrix);
	//cgeWindowOrtho(window, 1.0f, perspMatrix);
	cgeWindowPerspective(window, 0.5f, 1.0f, 100.0f, perspMatrix);

	struct cge3DRenderer* renderer = cge3DCreateRenderer(shader, &cameraMatrix, &perspMatrix);

	struct cge3DMesh* mesh = cgeLoadObj("build/teapot.obj", "build/gruen.png");

	//cgeUpdate3DMesh(mesh, vertices, indices, verticesCount, indicesCount);

	cgeTranslateRotate3DMesh(mesh, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 90.0f);
	cge3DRendererSetDiffuseDirection(renderer, 0.0f, 0.5f, 1.0f);

	float camX = 0.0f;
	float camY = 0.0f;
	float camZ = 25.0f;
	float camAngle = 90.0f;

	while(glfwWindowShouldClose(window) == 0) {
		glfwPollEvents();

		if(esc_pushed) {
			glfwSetWindowShouldClose(window, 1);
		}

		if(w_pushed) {
			camY = camY + 0.2f;
		} 
		if(a_pushed) {
			camX = camX - 0.2f;
		}
	        if(s_pushed) {
			camY = camY - 0.2f;
		}
		if(d_pushed) {
			camX = camX + 0.2f;
		}
		if(f5_pushed) {
			camZ = 25.0f;
			camAngle = camAngle - 10.0f;
			if(camAngle <= 10.0f) {
				camAngle = 90.0f;
			}
			f5_pushed = 0;
		}

		cgeWindowCamera(camX, camY, camZ, camAngle, cameraMatrix);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cge3DRendererRender(renderer, mesh);

		glfwSwapBuffers(window);
	}

	cgeDestroy3DMesh(mesh);
	cge3DDestroyRenderer(renderer);
	cgeDestroyWindow(window);	

	return 0;
}
