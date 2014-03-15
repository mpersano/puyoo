CROSS=mipsel-unknown-elf-

PSXSRC = $(SRC)/platform/$(PLATFORM)
PSXOBJ = $(OBJ)/platform/$(PLATFORM)

OBJDIRS += $(PSXOBJ)

PSXSDK_ROOT = /opt/psxsdk

OBJS += \
	$(PSXSRC)/new.o \
	$(PSXSRC)/draw_list.o \
	$(PSXSRC)/sprite.o \
	$(PSXSRC)/texture.o \
	$(PSXSRC)/main.o

CXXFLAGS += \
	-fsigned-char \
	-msoft-float \
	-mno-gpopt \
	-fno-builtin \
	-fno-rtti \
	-fno-exceptions \
	-fno-threadsafe-statics \
	-fno-use-cxa-atexit \
	-G0 \
	-I$(PSXSDK_ROOT)/include

LDFLAGS += \
	-nostdlib \
	-lgcc \
	-T $(PSXSDK_ROOT)/mipsel-unknown-elf/lib/ldscripts/playstation.x

CD_IMAGE_ROOT = cdroot
LICENSE_FILE  = $(PSXSDK_ROOT)/share/licenses/infoeur.dat
MKISOFS = genisoimage

$(TARGET).exe: $(TARGET)
	elf2exe $(TARGET) $(TARGET).exe

cdimage: $(TARGET).exe
	mkdir -p $(CD_IMAGE_ROOT)
	cp $(TARGET).exe sprites.tga sprites.spr $(CD_IMAGE_ROOT)
	systemcnf $(TARGET).exe > $(CD_IMAGE_ROOT)/system.cnf
	$(MKISOFS) -o $(TARGET).hsf -V $(TARGET) -sysid PLAYSTATION $(CD_IMAGE_ROOT)
	mkpsxiso $(TARGET).hsf $(TARGET).bin $(LICENSE_FILE)
	rm -f $(TARGET).hsf