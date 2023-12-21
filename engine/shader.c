#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>

#include <cge/shader.h>

struct cgeShader* cgeCreateShader() {
	struct cgeShader* shader = malloc(sizeof(struct cgeShader));
	
	shader->PROGRAM_ID = glCreateProgram();
	shader->shaderCount = 0;
	shader->shaders = NULL;

	return shader;
}

int cgeAttachShaderFromFile(struct cgeShader* shader, char* filepath, unsigned int shadertype) {
	FILE* shf = fopen((const char*) filepath, "r");

	if(shf == NULL) {
		fprintf(stderr, "Could not open shader file %s\n", filepath);
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}
	
	fseek(shf, 0L, SEEK_END);
	unsigned int size = ftell(shf);
	fseek(shf, 0L, SEEK_SET);

	char* shcont = malloc((size + 1) * sizeof(char));
	fread(shcont, 1, size, shf);
	fclose(shf);

	shcont[size] = '\0';

	int success = cgeAttachShader(shader, shcont, shadertype);
	free(shcont);

	return success;
}

int cgeAttachShader(struct cgeShader* shader, char* source, unsigned int shadertype) {
	unsigned int shid = glCreateShader(shadertype);
	glShaderSource(shid, 1, (const char* const*) &source, NULL);
	glCompileShader(shid);
	
	int shaderStatus = 0;
	glGetShaderiv(shid, GL_COMPILE_STATUS, &shaderStatus);
	if(shaderStatus == GL_FALSE) {
		int length;
		glGetShaderiv(shid, GL_INFO_LOG_LENGTH, &length);
		char* logBuffer = malloc(sizeof(char) * length);
		glGetShaderInfoLog(shid, length, &length, logBuffer);
		fprintf(stderr, "Shader %d\n", shadertype);
		fprintf(stderr, "%s\n", logBuffer);
		free(logBuffer);
		return -1;
	}
	
	shader->shaderCount = shader->shaderCount + 1;
	shader->shaders = realloc(shader->shaders, shader->shaderCount);
	shader->shaders[shader->shaderCount - 1] = shid;

	return 0;
}

int cgeMakeShader(struct cgeShader* shader) {
	for(unsigned int i = 0; i < shader->shaderCount; i++) {
		glAttachShader(shader->PROGRAM_ID, shader->shaders[i]);
	}
	glLinkProgram(shader->PROGRAM_ID);

	int shaderStatus = 0;
	glGetProgramiv(shader->PROGRAM_ID, GL_LINK_STATUS, &shaderStatus);
	if(shaderStatus == GL_FALSE) {
		int length;
                glGetProgramiv(shader->PROGRAM_ID, GL_INFO_LOG_LENGTH, &length);
                char* logBuffer = malloc(sizeof(char) * length);
                glGetProgramInfoLog(shader->PROGRAM_ID, length, &length, logBuffer);
		fprintf(stderr, "Could not link shader program.\n");
		fprintf(stderr, "%s\n", logBuffer);
		return -1;
	}

	return 0;
}

void cgeDestroyShader(struct cgeShader* shader) {
	glDeleteProgram(shader->PROGRAM_ID);
	
	free(shader->shaders);
	free(shader);
}
