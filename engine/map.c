#include <cge/map.h>
#include <cge/misc.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct cgeMapDirectory* cgeLoadMapDirectory(char* filepath) {
	FILE* mapDirectoryFile = fopen(filepath, "rb");
	
	

	return NULL; //TODO actually implement this thing here
}

void cgeSaveMapDirectory(struct cgeMapDirectory* directory, char* filepath) {

	fclose(directory->mapFile);
	return; //TODO actually implement this thing here uwu
}

void cgeUnloadMapDirectory(struct cgeMapDirectory* directory) {
	if(directory->openMap) {
		for(short i = 0; i < directory->mapCount; i++) {
			if(strcmp(directory->maps[i]->name, directory->openMap) == 0) {
				free(directory->maps[i]); //idiot check
			}
		}
	}

	fclose(cgeMapDirectory->mapFile);
	free(directory);
}

struct cgeMap* cgeLoadMapByName(struct cgeMapDirectory* directory, char* name) {
	for(short i = 0; i < directory->mapCount; i++) {
		if(strcmp(directory->maps[i]->name, directory->openMap) == 0) {
			return directory->maps[i];
		}
	}

	return NULL;
}

void cgePlaceMap(struct cgeMapDirectory* directory, struct cgeMap* map) {
	directory->mapCount = directory->mapCount + 1;
	directory->maps = realloc(directory->maps, sizeof(struct cgeMap*) * directory->mapCount);
	if(!directory->maps) {
		cgeCrash("Out of memory: could not place map!");
	}

	directory->maps[directory->mapCount - 1] = map;
}

void cgeUnloadMap(struct cgeMap* map) {
	//TODO execute map unloading scripts

	free(map);
}

struct cgeMapChunk* cgeGetMapChunk(struct cgeMap* map, short x, short y, short z) {
	for(int i = 0; i < map->chunkCount; i++) {
		if(map->chunks[i]->chunkPosX == x && map->chunks[i]->chunkPosY == y && map->chunks[i]->chunkPosZ == z) {
			return map->chunks[i];
		}
	}
	return NULL;
}

void cgePlaceMapChunk(struct cgeMap* map, struct cgeMapChunk* chunk) {
	map->chunkCount = map->chunkCount + 1;
        map->chunks = realloc(map->chunks, sizeof(struct cgeMapChunk*) * map->chunkCount);
        if(!map->chunks) {
                cgeCrash("Out of memory: could not place chunk!");
        }

        map->chunks[map->chunkCount - 1] = chunk;
}

void cgeUnloadMapChunk(struct cgeMapChunk* chunk) {
	free(chunk);
}

