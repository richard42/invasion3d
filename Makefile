#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Invasion3D - Makefile                                                 *
# *   Homepage: http://code.google.com/p/invasion3d/                        *
# *   Copyright (C) 2011 Richard Goedeken                                   *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# Makefile for Invasion3D

# detect operation system
UNAME = $(shell uname -s)
OS := NONE
ifeq ("$(UNAME)","Linux")
  OS = LINUX
endif
ifeq ("$(UNAME)","linux")
  OS = LINUX
endif
ifneq ("$(filter GNU hurd,$(UNAME))","")
  OS = LINUX
endif
ifeq ("$(UNAME)","Darwin")
  OS = OSX
endif
ifeq ("$(UNAME)","FreeBSD")
  OS = FREEBSD
endif
ifeq ("$(UNAME)","IRIX64")
  OS = IRIX
endif
ifneq ("$(filter GNU/kFreeBSD kfreebsd,$(UNAME))","")
  OS = LINUX
endif
ifeq ("$(OS)","NONE")
  $(error OS type "$(UNAME)" not supported.  Please file bug report at 'http://code.google.com/p/invasion3d/issues')
endif

# detect system architecture
HOST_CPU ?= $(shell uname -m)
CPU := NONE
ifneq ("$(filter x86_64 amd64,$(HOST_CPU))","")
  CPU := X86
  ifeq ("$(BITS)", "32")
    ARCH_DETECTED := 64BITS_32
  else
    ARCH_DETECTED := 64BITS
    NO_ASM := 1
  endif
endif
ifneq ("$(filter pentium i%86,$(HOST_CPU))","")
  CPU := X86
  ARCH_DETECTED := 32BITS
endif
ifneq ("$(filter ppc powerpc,$(HOST_CPU))","")
  CPU := PPC
  ARCH_DETECTED := 32BITS
  NO_ASM := 1
endif
ifneq ("$(filter ppc64 powerpc64,$(HOST_CPU))","")
  CPU := PPC
  ARCH_DETECTED := 64BITS
  NO_ASM := 1
endif
ifeq ("$(UNAME)","IRIX64")
  CPU := MIPS
  ARCH_DETECTED := 64BITS
  NO_ASM := 1
  CC = C99
  CXX = CC
  CFLAGS += -64 -I/usr/freeware/include -DCPU_BIG_ENDIAN
  LDFLAGS += -64 -L/usr/freeware/lib64 -lm -lpthread
endif
ifeq ("$(CPU)","NONE")
  $(error CPU type "$(HOST_CPU)" not supported.  Please file bug report at 'http://code.google.com/p/invasion3d/issues')
endif

# set various directory paths
WKDPATH	= $(shell pwd)
SRCPATH = $(WKDPATH)/src
OBJPATH = $(WKDPATH)/obj
INCPATH	= $(WKDPATH)/include
EXEPATH	= $(WKDPATH)/bin
DATAPATH = $(WKDPATH)/data

# base CFLAGS, LIBS, and LDFLAGS
CFLAGS += -ffast-math -funroll-loops -fexpensive-optimizations -fno-strict-aliasing -I$(INCPATH)
LDFLAGS += -O3 -L/usr/X11R6/lib -lSDL -lGL -lGLU
ASFLAGS += -f elf -DCFG_LINUX

# set special flags per-system
ifeq ($(OS), LINUX)
  ifeq ($(CPU), X86)
    ifeq ($(ARCH_DETECTED), 64BITS)
      CFLAGS += -pipe -O3 -march=athlon64
      LDFLAGS += -ldl
    else
      CFLAGS += -pipe -O3 -mmmx -msse -march=i686 -mtune=pentium-m -fomit-frame-pointer
      LDFLAGS += -ldl
    endif
  endif
endif
ifeq ($(OS), OSX)
  # The mac version of SDL requires inclusion of SDL_main in the executable
  LDFLAGS += -ldl $(shell sdl-config --libs)
  ifeq ($(CPU), X86)
    ifeq ($(ARCH_DETECTED), 64BITS)
      CFLAGS += -pipe -O3 -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
      LDFLAGS += -arch x86_64
    else
      CFLAGS += -pipe -O3 -mmmx -msse -fomit-frame-pointer -arch i686 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
      LDFLAGS += -arch i686
    endif
  endif
endif
ifeq ($(CPU), PPC)
  CFLAGS += -mcpu=powerpc
endif

# tweak flags for 32-bit build on 64-bit system
ifeq ($(ARCH_DETECTED), 64BITS_32)
  ifeq ($(OS), FREEBSD)
    $(error Do not use the BITS=32 option with FreeBSD, use -m32 and -m elf_i386)
  endif
  CFLAGS += -m32
  LDFLAGS += -m32
endif

# set shell function names
CC      ?= gcc
CXX     ?= g++
ASM     ?= nasm
CP      ?= cp
INSTALL ?= install
ifeq ($(OS),OSX)
  STRIP	?= strip -x 
else
  STRIP	?= strip -s
endif

# set special flags for given Makefile parameters
ifeq ($(DEBUG),1)
  CFLAGS += -g
  STRIP = true # disable binary strip
endif
ifeq ($(NO_ASM),1)
  CFLAGS += -DNO_SSE -DNO_MMX
endif

# set installation options
ifeq ($(PREFIX),)
  PREFIX := /usr/local
endif
ifeq ($(BINDIR),)
  BINDIR := $(PREFIX)/bin
endif
#added by to to allow for alternate media directory
ifeq ($(MEDIADIR),)
  MEDIADIR := $(BINDIR)
endif
#added so removal can be safer
MEDIAFILE = Invaders.dat
# list of source files to compile
SOURCE = \
	$(SRCPATH)/Bunker.cpp \
	$(SRCPATH)/Demo.cpp \
	$(SRCPATH)/Errr.cpp \
	$(SRCPATH)/GameLogic.cpp \
	$(SRCPATH)/GameMain.cpp \
	$(SRCPATH)/Gameplay.cpp \
	$(SRCPATH)/Ignignokt.cpp \
	$(SRCPATH)/Intro.cpp \
	$(SRCPATH)/Invader.cpp \
	$(SRCPATH)/Invader10.cpp \
	$(SRCPATH)/Invader20.cpp \
	$(SRCPATH)/Invader30.cpp \
	$(SRCPATH)/InvaderMystery.cpp \
	$(SRCPATH)/LaserBase.cpp \
	$(SRCPATH)/Package.cpp \
	$(SRCPATH)/Particles.cpp \
	$(SRCPATH)/Settings.cpp \
	$(SRCPATH)/Sound.cpp \
	$(SRCPATH)/TextGL.cpp \
	$(SRCPATH)/Textures.cpp \
	$(SRCPATH)/Ticker.cpp \
	$(SRCPATH)/Weapons.cpp
ifneq ($(NO_ASM),1)
SOURCE += $(SRCPATH)/VectorCode.asm
endif

# generate a list of object files build, make a temporary directory for them
OBJECTS := $(patsubst $(SRCPATH)/%.cpp, $(OBJPATH)/%.o, $(filter %.cpp, $(SOURCE)))
OBJECTS += $(patsubst $(SRCPATH)/%.asm, $(OBJPATH)/%.o, $(filter %.asm, $(SOURCE)))
OBJDIRS = $(dir $(OBJECTS))
$(shell mkdir -p $(OBJDIRS) $(EXEPATH))

# build targets
TARGET = invasion3d

targets:
	@echo "Invasion3D makefile."
	@echo "  Targets:"
	@echo "    all            == Build Invasion3D application"
	@echo "    clean          == remove object files and build products"
	@echo "    rebuild        == clean and re-build all"
	@echo "    install        == Install Invasion3D application"
	@echo "    uninstall      == Uninstall Invasion3D application"
	@echo "  Options:"
	@echo "    NO_ASM=1       == disable MMX assembly language optimizations"
	@echo "    BITS=32        == build 32-bit binary on 64-bit machine"
	@echo "    V=1            == display full commands when compiling"
	@echo "  Install Options:"
	@echo "    BINDIR=path    == install/uninstall location (default: /usr/local/bin)"
	@echo "    MEDIADIR=path  == install/uninstall location of game data (default: /usr/local/bin)"
	@echo "    DESTDIR=path   == path to prepend to all installation paths (only for packagers)"
	@echo "  Debugging Options:"
	@echo "    DEBUG=1        == add debugging symbols to application binary"

all: $(EXEPATH)/$(TARGET)

clean:
	rm -rf $(OBJPATH)/* $(EXEPATH)/*
	rmdir $(OBJPATH) $(EXEPATH)

rebuild: clean all

install: $(EXEPATH)/$(TARGET)
	$(INSTALL) -d -v "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -m 0755 $(EXEPATH)/$(TARGET) "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -d -v "$(DESTDIR)$(MEDIADIR)"
	$(INSTALL) -m 0644 $(DATAPATH)/$(MEDIAFILE) "$(DESTDIR)$(MEDIADIR)"
	$(INSTALL) -d -v "$(DESTDIR)/usr/share/pixmaps"
	$(INSTALL) -m 0644 $(DATAPATH)/Invasion3D.png "$(DESTDIR)/usr/share/pixmaps/invasion3d.png"
	$(INSTALL) -d -v "$(DESTDIR)/usr/share/applications"
	$(INSTALL) -m -0644 "$(DATAPATH)/invasion3d.desktop" "$(DESTDIR)/usr/share/applications"

uninstall:
	rm -f "$(DESTDIR)$(BINDIR)/$(TARGET)"
	rm -f "$(DESTDIR)$(MEDIADIR)/$(MEDIAFILE)"

# build dependency files
CFLAGS += -MD
-include $(OBJECTS:.o=.d)

# reduced compile output when running make without V=1
ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
	Q_CC  = @echo '    CC  '$@;
	Q_LD  = @echo '    LD  '$@;
	Q_ASM = @echo '    AS  '$@;
endif
endif

# build rules
$(EXEPATH)/$(TARGET): $(OBJECTS)
	$(Q_LD)$(CXX) $^ $(LDFLAGS) $(SRC_LIBS) -o $@
	$(STRIP) $@
	$(CP) $(DATAPATH)/$(MEDIAFILE) $(EXEPATH)

$(OBJECTS): Makefile

$(OBJPATH)/%.o: $(SRCPATH)/%.cpp
	$(Q_CC)$(CXX) -o $@ $(CFLAGS) -D BIN_DIR=\"${BINDIR}\" -D MEDIA_DIR=\"${MEDIADIR}\" -c $<

$(OBJPATH)/%.o: $(SRCPATH)/%.asm
	$(Q_ASM)$(ASM) $(ASFLAGS) $< -o $@


