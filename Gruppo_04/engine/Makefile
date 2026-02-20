CXX = cl.exe
LD = link.exe

# Aggiunto /D "FREEGLUT_STATIC" per dire a C++ che non stiamo usando una DLL per FreeGLUT
INC = /I"..\dependencies\freeglut\include" /I"..\dependencies\glm\include" /I"..\dependencies\glew\include" /I"..\dependencies\freeimage\include"
CFLAGS = /nologo /W3 /EHsc /std:c++20 /D "ENGINE_EXPORTS" /D "FREEGLUT_STATIC"

LIBS_COMMON = opengl32.lib glu32.lib glew32.lib FreeImage.lib

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

# --- DEBUG ---
# Aggiunto /MDd per allineare la libreria di runtime a quella di FreeImage/FreeGLUT
CFLAGS_DEBUG = $(CFLAGS) /Zi /Od /D "_DEBUG" /MDd
LIB_DIRS_DEBUG = /LIBPATH:"..\dependencies\freeglut\lib\x64\Debug" /LIBPATH:"..\dependencies\freeimage\lib\x64\Debug" /LIBPATH:"..\dependencies\glew\lib\Release\x64"
LIBS_DEBUG = $(LIBS_COMMON) freeglut_staticd.lib
LDFLAGS_DEBUG = /DLL /DEBUG $(LIB_DIRS_DEBUG)
OBJDIR_DEBUG = obj\Debug
OUTDIR_DEBUG = bin\Debug
OUT_DEBUG = $(OUTDIR_DEBUG)\engine.dll

# --- RELEASE ---
# Aggiunto /MD per la versione Release
CFLAGS_RELEASE = $(CFLAGS) /O2 /D "NDEBUG" /MD
LIB_DIRS_RELEASE = /LIBPATH:"..\dependencies\freeglut\lib\x64\Release" /LIBPATH:"..\dependencies\freeimage\lib\x64\Release" /LIBPATH:"..\dependencies\glew\lib\Release\x64"
LIBS_RELEASE = $(LIBS_COMMON) freeglut_static.lib
LDFLAGS_RELEASE = /DLL $(LIB_DIRS_RELEASE)
OBJDIR_RELEASE = obj\Release
OUTDIR_RELEASE = bin\Release
OUT_RELEASE = $(OUTDIR_RELEASE)\engine.dll

all: debug release

before_debug:
	@if not exist $(OUTDIR_DEBUG) mkdir $(OUTDIR_DEBUG)
	@if not exist $(OBJDIR_DEBUG) mkdir $(OBJDIR_DEBUG)

before_release:
	@if not exist $(OUTDIR_RELEASE) mkdir $(OUTDIR_RELEASE)
	@if not exist $(OBJDIR_RELEASE) mkdir $(OBJDIR_RELEASE)

# --- TARGET DEBUG ---
debug: before_debug $(OUT_DEBUG)

$(OUT_DEBUG):
	$(CXX) $(CFLAGS_DEBUG) $(INC) /c $(SRCS) /Fo"$(OBJDIR_DEBUG)/"
	$(LD) /nologo $(LDFLAGS_DEBUG) /OUT:$(OUT_DEBUG) /IMPLIB:$(OUTDIR_DEBUG)\engine.lib $(OBJDIR_DEBUG)\*.obj $(LIBS_DEBUG)

# --- TARGET RELEASE ---
release: before_release $(OUT_RELEASE)

$(OUT_RELEASE):
	$(CXX) $(CFLAGS_RELEASE) $(INC) /c $(SRCS) /Fo"$(OBJDIR_RELEASE)/"
	$(LD) /nologo $(LDFLAGS_RELEASE) /OUT:$(OUT_RELEASE) /IMPLIB:$(OUTDIR_RELEASE)\engine.lib $(OBJDIR_RELEASE)\*.obj $(LIBS_RELEASE)

clean:
	@if exist obj rmdir /s /q obj
	@if exist bin rmdir /s /q bin

test:
	@echo Test non ancora configurato per Windows nativo.