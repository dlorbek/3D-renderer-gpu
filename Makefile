all:
	g++ -Iinclude -Iinclude/SDL3 -Iinclude/SDL3_image -Iinclude/headers -Llib resources.res -o main src/*.cpp src/*.c -lSDL3 -lopengl32 -lole32 -luuid

nocmd:
	g++ -Iinclude -Iinclude/SDL3 -Iinclude/SDL3_image -Iinclude/headers -Llib resources.res -o 3D-Viewer src/*.cpp src/*.c -lSDL3 -lopengl32 -lole32 -luuid -mwindows

resources:
	windres resources.rc -O coff -o resources.res
