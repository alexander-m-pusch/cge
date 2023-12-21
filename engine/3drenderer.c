#include <cge/3drenderer.h>
#include <GL/glew.h>

#include <string.h>
#include <stdlib.h>

struct cge3DRenderer* cge3DCreateRenderer(struct cgeShader* shader, mat4* viewMatrix, mat4* projectionMatrix) {
	struct cge3DRenderer* renderer = malloc(sizeof(struct cge3DRenderer));

	renderer->shader = shader;
	renderer->viewMatrix = viewMatrix;
	renderer->projectionMatrix = projectionMatrix;

	renderer->ambientLocation[0] = 0.0f;
	renderer->ambientLocation[1] = 0.0f;
	renderer->ambientLocation[2] = 0.0f;

	renderer->drawMode = GL_TRIANGLES;

	return renderer;
}

void cge3DRendererSetDrawMode(struct cge3DRenderer* renderer, char* mode) {
	if(strcmp(mode, "normal")) renderer->drawMode = GL_TRIANGLES;
	if(strcmp(mode, "wireframe")) renderer->drawMode = GL_LINES;
}

void cge3DRendererSetDiffuseDirection(struct cge3DRenderer* renderer, float posX, float posY, float posZ) {
	renderer->ambientLocation[0] = posX;
	renderer->ambientLocation[1] = posY;
	renderer->ambientLocation[2] = posZ;
}

void cge3DRendererRender(struct cge3DRenderer* renderer, struct cge3DMesh* mesh) {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glUseProgram(renderer->shader->PROGRAM_ID);

	mat4 modelMatrixInverse;
	glm_mat4_inv(mesh->modelMatrix, modelMatrixInverse);
	glm_mat4_transpose(modelMatrixInverse);

	glUniformMatrix4fv(glGetUniformLocation(renderer->shader->PROGRAM_ID, "viewMatrix"), 1, 0, (float*) renderer->viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(renderer->shader->PROGRAM_ID, "projectionMatrix"), 1, 0, (float*) renderer->projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(renderer->shader->PROGRAM_ID, "modelMatrix"), 1, 0, (float*) mesh->modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(renderer->shader->PROGRAM_ID, "modelMatrixInverse"), 1, 0, (float*) modelMatrixInverse);
	glUniform3fv(glGetUniformLocation(renderer->shader->PROGRAM_ID, "ambient"), 1, (float*) renderer->ambientLocation);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->TEXTURE->texture);

	glBindVertexArray(mesh->VAO_ID);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawElements(renderer->drawMode, mesh->size, GL_UNSIGNED_INT, NULL);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_CULL_FACE);
}

void cge3DDestroyRenderer(struct cge3DRenderer* renderer) {
	free(renderer);
}
