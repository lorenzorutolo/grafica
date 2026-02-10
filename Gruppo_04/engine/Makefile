CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I../dependencies/freeglut/include -I../dependencies/glm/include -I../dependencies/glew/include
CFLAGS = -Wall -std=c++20 -fPIC -m64 -fexceptions
RCFLAGS = 
RESINC = 
LIBDIR = 
LIB = -lglut -lGL -lGLU -lfreeimage -lGLEW
LDFLAGS = -m64

# Engine sources
SRCS = attenuationLight.cpp \
       camera.cpp \
       directionalLight.cpp \
       engine.cpp \
       light.cpp \
       list.cpp \
       material.cpp \
       mesh.cpp \
       node.cpp \
       object.cpp \
       omnidirectionalLight.cpp \
       ortographicCamera.cpp \
       ovoreader.cpp \
       perspectiveCamera.cpp \
       spotLight.cpp \
       texture2D.cpp

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g -D_DEBUG
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/libengine.so
OBJDIR_TEST = obj/Test
OUT_TEST = bin/Debug/engine_tests

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/libengine.so

OBJ_DEBUG = $(SRCS:%.cpp=$(OBJDIR_DEBUG)/%.o)

OBJ_RELEASE = $(SRCS:%.cpp=$(OBJDIR_RELEASE)/%.o)

all: debug release

test: debug before_test $(OUT_TEST)
	LD_LIBRARY_PATH=bin/Debug ./$(OUT_TEST)

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) -shared $(LIBDIR_DEBUG) $(OBJ_DEBUG)  -o $(OUT_DEBUG) $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/%.o: %.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

before_test:
	test -d $(OBJDIR_TEST) || mkdir -p $(OBJDIR_TEST)

$(OUT_TEST): $(OBJDIR_TEST)/engine_tests.o $(OUT_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -Lbin/Debug $(OBJDIR_TEST)/engine_tests.o -o $(OUT_TEST) $(LDFLAGS_DEBUG) $(LIB_DEBUG) -lengine -Wl,-rpath,'$$ORIGIN'

$(OBJDIR_TEST)/engine_tests.o: tests/engine_tests.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -I. -c tests/engine_tests.cpp -o $(OBJDIR_TEST)/engine_tests.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG) $(OUT_TEST) $(OBJDIR_TEST)/engine_tests.o
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_TEST)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) $(OBJ_RELEASE)  -o $(OUT_RELEASE) $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/%.o: %.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $< -o $@

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release before_test test
