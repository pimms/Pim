CXX=g++
FLGS=-g -D LINUX -D _DEBUG
DEPS=-lGL -lGLU
INCS=-Isrc/
SRCS=$(shell ls src/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	$(CXX) $(INCS) $(DEPS) -o pim.a $(OBJS) $(FLGS)

%.o: %.cpp
	$(CXX) $(INCS) $(DEPS) -o $@ -c $< $(FLGS)
