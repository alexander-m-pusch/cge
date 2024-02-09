#include <cge/mesh.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj/fast_obj.h>

struct cge3DMesh* cgeCreate3DMesh(struct cgeTexture* texture) {
	struct cge3DMesh* mesh = malloc(sizeof(struct cge3DMesh));
	
	glGenVertexArrays(1, &mesh->VAO_ID);
	glGenBuffers(1, &mesh->VBO_ID);
	glGenBuffers(1, &mesh->EBO_ID);

	mesh->TEXTURE = texture;

	glm_mat4_identity(mesh->modelMatrix);
	
	mesh->vertices = NULL;
	mesh->indices = NULL;

	return mesh;
}

struct cge3DMesh* cgeLoadObj(char* modelPath, char* texturePath) {
	fastObjMesh* mesh_raw = fast_obj_read(modelPath);

	struct cgeTexture* tex = cgeLoadTexture(texturePath);
	struct cge3DMesh* mesh = cgeCreate3DMesh(tex);

	unsigned int vc = mesh_raw->index_count * 8;
	unsigned int ic = mesh_raw->index_count;

	float* vertices = calloc(vc, sizeof(float));
	unsigned int* indices = calloc(ic, sizeof(unsigned int));

	for(int i = 0; i < ic; i++) {
		unsigned int pos = mesh_raw->indices[i].p;
		indices[i] = pos;
		if(mesh_raw->position_count * 3 >= pos) {
			vertices[pos * 8] = mesh_raw->positions[pos * 3];
			vertices[pos * 8 + 1] = mesh_raw->positions[pos * 3 + 1];
			vertices[pos * 8 + 2] = mesh_raw->positions[pos * 3 + 2];
		}

		unsigned int norm = mesh_raw->indices[i].n;
		if(mesh_raw->normal_count * 3 >= norm) {
			vertices[pos * 8 + 3] = mesh_raw->normals[norm * 3];
			vertices[pos * 8 + 4] = mesh_raw->normals[norm * 3 + 1];
			vertices[pos * 8 + 5] = mesh_raw->normals[norm * 3 + 2];
		}

		unsigned int tex = mesh_raw->indices[i].t;
		if(mesh_raw->texcoord_count * 2 >= tex) {
			vertices[pos * 8 + 6] = mesh_raw->texcoords[tex * 2];
			vertices[pos * 8 + 7] = mesh_raw->texcoords[tex * 2 + 1];
		}
	}

	fast_obj_destroy(mesh_raw);

	cgeUpdate3DMesh(mesh, vertices, indices, vc, ic);

	return mesh;
}

void cgeUpdate3DMesh(struct cge3DMesh* mesh, float* vertices, unsigned int* indices, unsigned int vc, unsigned int ic) {
	mesh->indices = indices;
	mesh->vertices = vertices;

	mesh->size = ic;

	glBindVertexArray(mesh->VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, vc * sizeof(float), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ic * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);         
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));         
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);	
	glBindVertexArray(0);
}

void cgeTranslateRotate3DMesh(struct cge3DMesh* mesh, float x, float y, float z, float axisAngleX, float axisAngleY, float axisAngleZ, float deg) {
	vec3 translationVector = {x, y, z};
	vec3 rotationAxis = {axisAngleX, axisAngleY, axisAngleZ};
	glm_mat4_identity(mesh->modelMatrix);
	glm_translate(mesh->modelMatrix, translationVector);
	glm_make_rad(&deg);
	glm_rotate(mesh->modelMatrix, deg, rotationAxis);
}

void cgeDestroy3DMesh(struct cge3DMesh* mesh) {
	glDeleteVertexArrays(1, &mesh->VAO_ID);
	glDeleteBuffers(1, &mesh->VBO_ID);
	glDeleteBuffers(1, &mesh->EBO_ID);
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh);
}
