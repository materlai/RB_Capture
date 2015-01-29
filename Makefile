

CC := g++
AR :=AR

INCLUDE_PATH :=

LIBS_PATH :=-L/usr/lib/SDL
LIBS := -lSDL2

CFLAGS := -c -Wall -O2 
CPPFLAGS := 

OBJECTS_DEPS :=SDL2.o v4l2.o colorspace.o  
CHK_SOURCES :=SDL2.cpp v4l2src.cpp colorspace.cpp

building_echo     := @echo 'building target:'
building_finished := @echo 'finished building target:'
Target_Object     :=RB_Capture

$(Target_Object): $(OBJECTS_DEPS) main.o 
	$(building_echo) $@
	$(CC) main.o $(OBJECTS_DEPS)    $(LIBS_PATH)  $(LIBS) -o $@
	$(building_finished) $@
v4l2.o:v4l2src.cpp  v4l2.h
	$(building_echo) $@
	$(CC) $(CFLAGS) v4l2src.cpp -o v4l2.o
	$(building_finished) $@

colorspace.o:colorspace.cpp colorspace.h
	$(building_echo)  $@
	$(CC) $(CFLAGS) colorspace.cpp -o colorspace.o
	$(building_finished) $@

SDL2.o:SDL2.cpp CSDL2.h
	$(building_echo)  $@
	$(CC) $(CFLAGS) SDL2.cpp  -o SDL2.o 
	$(building_finished) $@

main.o:main.cpp	
	$(building_echo)  $@
	$(CC) $(CFLAGS) main.cpp -o main.o
	$(building_finished) $@
check-syntax:
	gcc -o nul -S ${CHK_SOURCES}
clean:
	rm -rf $(Target_Object)  main.o $(OBJECTS_DEPS)

