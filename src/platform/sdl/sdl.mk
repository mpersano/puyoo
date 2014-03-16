SDLSRC = $(SRC)/platform/$(PLATFORM)
SDLOBJ = $(OBJ)/platform/$(PLATFORM)

OBJDIRS += $(SDLOBJ)

OBJS += \
	$(SDLOBJ)/main.o

CXXFLAGS += `pkg-config --cflags sdl gl glu`
LIBS += `pkg-config --libs sdl gl glu`
