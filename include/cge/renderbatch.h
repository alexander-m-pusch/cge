#ifndef CGE_RENDERBATCH_H
#define CGE_RENDERBATCH_H

#include <cglm/mat4.h>

struct cgeRenderBatch {
	float* vertices;
	unsigned int* indices;
	char* flags;

	mat4 viewMatrix;
	mat4 cameraMatrix;
	
	int SIZE;

	unsigned int VAO_ID;
	unsigned int VBO_ID;
	unsigned int EBO_ID;

	unsigned int SHADER_ID;
	unsigned int SPRITESHEET_ID;
};

struct cgeRenderBatch* cgeCreateRenderBatch(unsigned int size, unsigned int shader, unsigned int spritesheet);

unsigned int cgeRenderBatchTileAlloc(struct cgeRenderBatch* batch);
void cgeRenderBatchTileFree(struct cgeRenderBatch* batch, unsigned int index);
void cgeRenderBatchTileUpdate(struct cgeRenderBatch* batch, unsigned int index);

void cgeDestroyRenderBatch(struct cgeRenderBatch* batch);

void cgeRenderRenderBatch(struct cgeRenderBatch* batch);

#endif
