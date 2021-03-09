CC = g++
CFLAGS = -Wno-deprecated-declarations -Wno-shift-op-parentheses -Xpreprocessor -fopenmp
INCFLAGS = -I./glm-0.9.7.1 -I/usr/local/opt/llvm/include -I./include/ -std=c++11
LDFLAGS = -L./lib/mac/ -L/usr/local/opt/llvm/lib -lm -lfreeimage -lomp
RM = /bin/rm -f

all: raytracer

raytracer: main.o Camera.o Ray.o Transform.o Geometry.o
	$(CC) -o raytracer main.o Camera.o Ray.o Transform.o Geometry.o $(INCFLAGS) $(CFLAGS) $(LDFLAGS)

main.o: main.cpp Camera.o Ray.o Transform.o Geometry.o
	$(CC) $(INCFLAGS) -c main.cpp -lfreeimage

Camera.o: Camera.cpp Camera.hpp Transform.h Geometry.hpp Ray.hpp
	$(CC)  $(INCFLAGS) -c Camera.cpp

Ray.o: Ray.cpp Ray.hpp Transform.h
	$(CC) $(INCFLAGS) -c Ray.cpp

Transform.o: Transform.cpp Transform.h
	$(CC) $(INCFLAGS) -c Transform.cpp

Geometry.o: Geometry.cpp Geometry.hpp Transform.h Ray.hpp
	$(CC) $(INCFLAGS) -c Geometry.cpp

clean:
	$(RM) *.o raytracer *.png


 
