#ifndef THREEDRENDERER_H_
#define THREEDRENDERER_H_

#include <cge/mesh.h>
#include <cge/shader.h>
#include <cglm/mat4.h>

struct cge3DRenderer {
	struct cgeShader* shader;

	unsigned int drawMode;

	mat4* viewMatrix;
	mat4* projectionMatrix;

	vec3 ambientLocation;
};

struct cge3DRenderer* cge3DCreateRenderer(struct cgeShader* shader, mat4* viewMatrix, mat4* projectionMatrix);
void cge3DRendererSetDiffuseDirection(struct cge3DRenderer* renderer, float posX, float posY, float posZ);
void cge3DRendererSetDrawMode(struct cge3DRenderer* renderer, char* mode);
void cge3DRendererRender(struct cge3DRenderer* renderer, struct cge3DMesh* mesh);
void cge3DDestroyRenderer(struct cge3DRenderer* renderer);

#endif
