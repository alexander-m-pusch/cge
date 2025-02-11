#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

#define STRINGIFY_RAW(A) #A
#define STRINGIFY(A) STRINGIFY_RAW(A)

void cgeCrash(char* text) {
	fprintf(stderr, "########################################\n");
	fprintf(stderr, "CGE has crashed: %s\n", text);
	fprintf(stderr, "########################################\n");
	fprintf(stderr, "Backtrace: \n");
	void* callstack[128];
	int i, frames = backtrace(callstack, 128);
	char** symbols = backtrace_symbols(callstack, frames);
	for(i = 0; i < frames; ++i) {
		fprintf(stderr, "%s\n", symbols[i]);
	}

	free(symbols);
	exit(1);
}

char* cgeVersion() {
	return STRINGIFY(CGE_VERSION_MAJOR) "." STRINGIFY(CGE_VERSION_MINOR) "." STRINGIFY(CGE_VERSION_PATCH);
}

char* cgeBuild() {
	return STRINGIFY(CGE_BUILD_TYPE);
}
