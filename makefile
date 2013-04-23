CXX=g++
FLGS=-g -std=c++0x -DLINUX -DUNIX -D_DEBUG -DGL_GLEXT_PROTOTYPES
DEPS=-lGL -lGLU -lopenal -lSDL2 -lpng -lvorbis
INCS=-Isrc/ -I/usr/local/include/freetype2/ -Isrc/dep/tinyxml/
SRCS=$(shell ls src/*.cpp) $(shell ls src/dep/tinyxml/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

libpim.so: $(OBJS)
	ar rvs libpim.so $(OBJS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(INCS) $(DEPS) $(FLGS) 

install: libpim.so
	rm -rf ../pimux
	mkdir ../pimux
	mkdir ../pimux/include
	cp libpim.so ../pimux
	cp -r bin/include/* ../pimux/include/

clean:
	@echo "Removing object files..."
	@rm $(OBJS)
	@rm pim.so
	@echo "Done!"