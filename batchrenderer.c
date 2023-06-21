#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

#include <cglm/mat4.h>
#include <cglm/cam.h>

#include <cge/renderbatch.h>

#define VERTEX_STRIDE 9

#define VERTEX_POSITION_SIZE 3
#define VERTEX_POSITION_OFFSET (GLvoid*) 0

#define VERTEX_COLOR_SIZE 4
#define VERTEX_COLOR_OFFSET (GLvoid*) (3 * sizeof(float))

#define VERTEX_TEXCOORDS_SIZE 2
#define VERTEX_TEXCOORDS_OFFSET (GLvoid*) (7 * sizeof(float))

struct cgeRenderBatch* cgeCreateRenderBatch(unsigned int size, unsigned int shader, unsigned int spritesheet) {
	struct cgeRenderBatch* batch = malloc(sizeof(struct cgeRenderBatch));
	
	batch->SIZE = size;
	batch->SHADER_ID = shader;
	
	glGenVertexArrays(1, &(batch->VAO_ID));
	glBindVertexArray(batch->VAO_ID);
	
	glGenBuffers(1, &(batch->VBO_ID));
	
	batch->vertices = calloc(size * 4 * VERTEX_STRIDE, sizeof(float));
	batch->indices = calloc(size * 6, sizeof(unsigned int));
	batch->flags = calloc(size, sizeof(char));

	glm_mat4_identity(batch->viewMatrix);
	glm_mat4_identity(batch->cameraMatrix);

	glGenBuffers(1, &(batch->EBO_ID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->EBO_ID);

	for(unsigned int i = 0; i < size; i++) {
		unsigned int offset = i * 4;
		unsigned int indexOffset = i * 6;
		batch->indices[indexOffset] = offset + 3;
		batch->indices[indexOffset + 1] = offset + 2;
		batch->indices[indexOffset + 2] = offset + 0;
		batch->indices[indexOffset + 3] = offset + 0;
		batch->indices[indexOffset + 4] = offset + 2;
		batch->indices[indexOffset + 5] = offset + 1;
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * size * 6, batch->indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, batch->VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_STRIDE * size * 4, batch->vertices, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, VERTEX_POSITION_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), VERTEX_POSITION_OFFSET);
	glVertexAttribPointer(1, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), VERTEX_COLOR_OFFSET);
	glVertexAttribPointer(2, VERTEX_TEXCOORDS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), VERTEX_TEXCOORDS_OFFSET);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return batch;
}

unsigned int cgeRenderBatchTileAlloc(struct cgeRenderBatch* batch) {
	for(int i = 0; i < batch->SIZE; i++) {
		char flags = batch->flags[i];
		if(flags == '\0') {
			return i;
		}
	}

	return batch->SIZE + 1;
}

void cgeRenderBatchTileFree(struct cgeRenderBatch* batch, unsigned int index) {
	batch->flags[index] = '\0';
}

void cgeRenderBatchTileUpdate(struct cgeRenderBatch* batch, unsigned int index) {
	glBindVertexArray(batch->VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, batch->VBO_ID);

	glBufferSubData(GL_ARRAY_BUFFER, 4 * VERTEX_STRIDE * sizeof(float) * index, 4 * VERTEX_STRIDE * sizeof(float), (batch->vertices + VERTEX_STRIDE * sizeof(float) * index));	
	//glBufferData(GL_ARRAY_BUFFER, VERTEX_STRIDE * sizeof(float) * batch->SIZE * 4, batch->vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void cgeDestroyRenderBatch(struct cgeRenderBatch* batch) {
	glDeleteBuffers(1, &(batch->VBO_ID));
	glDeleteBuffers(1, &(batch->EBO_ID));

	glDeleteVertexArrays(1, &(batch->VAO_ID));

	free(batch->vertices);
	free(batch->indices);
	free(batch);
}

void cgeRenderRenderBatch(struct cgeRenderBatch* batch) {
	glUseProgram(batch->SHADER_ID);
	glBindVertexArray(batch->VAO_ID);
	
	glUniformMatrix4fv(glGetUniformLocation(batch->SHADER_ID, "viewMatrix"), 1,  0, (float*) batch->viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(batch->SHADER_ID, "cameraMatrix"), 1, 0, (float*) batch->cameraMatrix);
	
	glActiveTexture(GL_TEXTURE0);
	
	if(batch->SPRITESHEET_ID) {
		glBindTexture(GL_TEXTURE_2D, batch->SPRITESHEET_ID);
	}
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, batch->SIZE * 6, GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindVertexArray(0);
	glUseProgram(0);
}
