all:
	g++ -Iinclude -Iinclude/SDL3 -Iinclude/SDL3_image -Iinclude/headers -Llib resources.res -o main src/*.cpp src/*.c -lSDL3 -lopengl32

nocmd:
	g++ -Iinclude -Iinclude/SDL3 -Iinclude/SDL3_image -Iinclude/headers -Llib resources.res -o 3D-Viewer src/*.cpp src/*.c -lSDL3 -lopengl32 -mwindows

resources:
	windres resources.rc -O coff -o resources.res
