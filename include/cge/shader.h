#ifndef CGE_SHADER_H_
#define CGE_SHADER_H_

struct cgeShader {
	unsigned int PROGRAM_ID;
	unsigned int shaderCount;
	unsigned int* shaders;
};

struct cgeShader* cgeCreateShader();
int cgeAttachShaderFromFile(struct cgeShader* shader, char* sourcefile, unsigned int shadertype);
int cgeAttachShader(struct cgeShader* shader, char* source, unsigned int shadertype);
int cgeMakeShader(struct cgeShader* shader);
void cgeDestroyShader(struct cgeShader* shader);

#endif
