CXX = cl.exe
LD = link.exe

# Aggiunto FREEGLUT_STATIC per dire agli header di non cercare la versione DLL
INC = /I"..\engine" /I"..\dependencies\glm\include" /I"..\dependencies\freeglut\include"
CFLAGS = /nologo /W3 /EHsc /std:c++20 /D "FREEGLUT_STATIC"

# Aggiungiamo i percorsi di FreeGLUT anche qui, perché gli header lo auto-richiedono
LIB_DIRS_DEBUG = /LIBPATH:"..\dependencies\freeglut\lib\x64\Debug"
LIB_DIRS_RELEASE = /LIBPATH:"..\dependencies\freeglut\lib\x64\Release"

# Il client linkerà l'engine + le dipendenze di sistema richieste da FreeGLUT
LIBS_DEBUG = ..\engine\bin\Debug\engine.lib freeglut_staticd.lib opengl32.lib glu32.lib
LIBS_RELEASE = ..\engine\bin\Release\engine.lib freeglut_static.lib opengl32.lib glu32.lib

# --- DEBUG ---
# Aggiunto /MDd
CFLAGS_DEBUG = $(CFLAGS) /Zi /Od /D "_DEBUG" /MDd
OBJDIR_DEBUG = obj\Debug
OUTDIR_DEBUG = bin\Debug
OUT_DEBUG = $(OUTDIR_DEBUG)\client.exe
OBJ_DEBUG = $(OBJDIR_DEBUG)\main.obj

# --- RELEASE ---
# Aggiunto /MD
CFLAGS_RELEASE = $(CFLAGS) /O2 /D "NDEBUG" /MD
OBJDIR_RELEASE = obj\Release
OUTDIR_RELEASE = bin\Release
OUT_RELEASE = $(OUTDIR_RELEASE)\client.exe
OBJ_RELEASE = $(OBJDIR_RELEASE)\main.obj

all: debug release

before_debug:
	@if not exist $(OUTDIR_DEBUG) mkdir $(OUTDIR_DEBUG)
	@if not exist $(OBJDIR_DEBUG) mkdir $(OBJDIR_DEBUG)

before_release:
	@if not exist $(OUTDIR_RELEASE) mkdir $(OUTDIR_RELEASE)
	@if not exist $(OBJDIR_RELEASE) mkdir $(OBJDIR_RELEASE)

# --- TARGET DEBUG ---
debug: before_debug $(OUT_DEBUG)

$(OUT_DEBUG): $(OBJ_DEBUG)
	$(LD) /nologo /DEBUG /OUT:$(OUT_DEBUG) $(LIB_DIRS_DEBUG) $(OBJ_DEBUG) $(LIBS_DEBUG)

$(OBJDIR_DEBUG)\main.obj: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC) /c main.cpp /Fo$(OBJDIR_DEBUG)\main.obj

# --- TARGET RELEASE ---
release: before_release $(OUT_RELEASE)

$(OUT_RELEASE): $(OBJ_RELEASE)
	$(LD) /nologo /OUT:$(OUT_RELEASE) $(LIB_DIRS_RELEASE) $(OBJ_RELEASE) $(LIBS_RELEASE)

$(OBJDIR_RELEASE)\main.obj: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC) /c main.cpp /Fo$(OBJDIR_RELEASE)\main.obj

clean:
	@if exist obj rmdir /s /q obj
	@if exist bin rmdir /s /q bin