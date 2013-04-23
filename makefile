CXX=g++
FLGS=-g -std=c++0x -DLINUX -DUNIX -D_DEBUG -DGL_GLEXT_PROTOTYPES
DEPS=-lGL -lGLU -lopenal -lSDL2 -lpng -lvorbis
INCS=-Isrc/ -I/usr/local/include/freetype2/ -Isrc/dep/tinyxml/
SRCS=$(shell ls src/*.cpp) $(shell ls src/dep/tinyxml/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

libpim.a: $(OBJS)
	@echo "Archiving libpim.a..."
	ar rvs libpim.a $(OBJS)
	@echo "Done!"

	@echo "Copying headers..."
	cp src/*.h bin/include/
	@echo "Done!"

%.o: %.cpp
	$(CXX) -o $@ -c $< $(INCS) $(DEPS) $(FLGS) 

install: libpim.a
	mkdir ../pimux
	mkdir ../pimux/include
	cp libpim.a ../pimux
	cp -r bin/include/* ../pimux/include/

clean:
	@echo "Removing object files..."
	@rm $(OBJS)
	@rm libpim.a
	@echo "Done!"