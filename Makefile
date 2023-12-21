.DEFAULT_GOAL := all

#Must be either CGE_RELEASE or CGE_DEBUG, otherwise weird stuff happens
CGE_BUILD=CGE_DEBUG
CGE_VERSION_MAJOR=0
CGE_VERSION_MINOR=1
CGE_VERSION_PATCH=0

engine:
	cc -fPIC -shared -Iengine/include -Icglm/include -lGLEW -lGL -lglfw -Wall -Wpedantic -DBUILD=$(CGE_BUILD) -DCGE_VERSION_MAJOR=$(CGE_VERSION_MAJOR) -DCGE_VERSION_MINOR=$(CGE_VERSION_MINOR) -DCGE_VERSION_PATCH=$(CGE_VERSION_PATCH) $(wildcard engine/*.c) -o build/libCGE.so

game: engine
	cc -Igame/include -Iengine/include -Icglm/include -Lbuild -lGLEW -lGL -lglfw -lCGE -Wall -Wpedantic -Wl,-rpath='$$ORIGIN' $(wildcard game/*.c) -o build/game

run: game
	build/game

clean:
	rm -rf build/
	mkdir build/

install:
	cp build/lib* /usr/lib/
	cp build/game /usr/bin/game

all: game

.PHONY: engine game run install
