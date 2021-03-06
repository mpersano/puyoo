ifndef PLATFORM
PLATFORM = sdl
endif

TARGET = game

SRC = src

OBJ = obj

OBJDIRS = $(OBJ)

CXXFLAGS = \
	-Wall \
	-g \
	-O2 \
	-std=c++0x

INCPATH = \
	-I$(SRC) \
	-I$(SRC)/platform/$(PLATFORM)

LIBS =

LDFLAGS =

OBJS = \
	$(OBJ)/game.o \
	$(OBJ)/grid.o \
	$(OBJ)/falling_block.o \
	$(OBJ)/combo_animation.o \
	$(OBJ)/image.o \
	$(OBJ)/sprite.o \
	$(OBJ)/font.o \
	$(OBJ)/file_reader.o \
	$(OBJ)/in_game.o

include $(SRC)/platform/$(PLATFORM)/$(PLATFORM).mk

CXX = $(CROSS)g++
LD = $(CROSS)g++
MD = mkdir

CXXFLAGS += $(INCPATH)

$(TARGET): maketree $(OBJS)
	$(LD) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

maketree: $(sort $(OBJDIRS))

$(sort $(OBJDIRS)):
	mkdir -p $@

$(OBJ)/%.o: $(SRC)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)
ifeq ($(PLATFORM),psx)
	rm -rf $(CD_IMAGE_ROOT) $(foreach EXT,hsf bin cue exe,$(TARGET).$(EXT))
endif

.PHONY: all clean
