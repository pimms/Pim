# Compiler Flags
CXX=g++
FLGS=-g -std=c++0x -fPIC

# Library Name
LIBTARGET=libpim.a

# Source directory
SRCDIR=src/

# Desination Include directory
DSTDIR=bin/include/

# Installation Root directory
INSTALLDIR=/usr/

# Preprocessor definitions
DEFS=-DLINUX -DUNIX -D_DEBUG -DGL_GLEXT_PROTOTYPES

# Library Dependencies
LIBS=-lopenal -lSDL2 -lpng -lvorbis -lvorbisfile -lfreetype -lGL -lGLU


# Include Directories
INCS=-Isrc/ -I/usr/local/include/freetype2/ -Isrc/dep/tinyxml/

# Source and Object files
SRCS=$(shell ls $(SRCDIR)*.cpp) $(shell ls $(SRCDIR)dep/tinyxml/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

# Old build:
#ar rvs $(LIBTARGET) $(OBJS)
#$(CXX) $(FLGS) -o $(LIBTARGET) $(OBJS) $(DEFS) $(LIBS)

libpim.a: $(OBJS)
	@echo "Archiving libpim.a..."
	@echo "Done!"
	ar rvs $(LIBTARGET) $(OBJS)
	@echo "Copying headers..."
	cp $(SRCDIR)*.h $(DSTDIR)
	@echo "Done!"

%.o: %.cpp
	@$(CXX) $(FLGS)  -o $@ -c $<  $(DEFS) $(INCS) $(LIBS)
	@echo "Compiling $<..."

install: $(LIBTARGET)
	@mkdir -p $(INSTALLDIR)include/Pim/

	@cp $(LIBTARGET) $(INSTALLDIR)lib/
	@cp -r $(DSTDIR)*.h $(INSTALLDIR)include/Pim/
	@echo "The headers can be found in: $(INSTALLDIR)include/Pim/."
	@echo "The library can be found in: $(INSTALLDIR)lib/."

clean:
	@echo "Removing object files..."
	@rm -f $(OBJS) $(LIBTARGET)
	@echo "Done!"
