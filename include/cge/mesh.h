#ifndef MESH_H_
#define MESH_H_

#include <cge/texture.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>

struct cge3DMesh {
	unsigned int VAO_ID;
	unsigned int VBO_ID;
	unsigned int EBO_ID;

	struct cgeTexture* TEXTURE;

	float* vertices;
	unsigned int* indices;

	mat4 modelMatrix;

	unsigned int size;
};

struct cge3DMesh* cgeCreate3DMesh(struct cgeTexture* texture);
struct cge3DMesh* cgeLoadObj(char* modelPath, char* texturePath);
void cgeUpdate3DMesh(struct cge3DMesh* mesh, float* vertices, unsigned int* indices, unsigned int vc, unsigned int ic);
void cgeTranslateRotate3DMesh(struct cge3DMesh* mesh, float x, float y, float z, float axisAngleX, float axisAngleY, float axisAngleZ, float deg);
void cgeDestroy3DMesh(struct cge3DMesh* mesh);

#endif
