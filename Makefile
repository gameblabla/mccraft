SRCDIR		= ./src ./src/pcfx
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
CD_OBJECTS	   = 
OBJECTS        = $(OBJ_C)
ELF_TARGET     = ncraft.elf
BIN_TARGET     = ncraft.bin
ADD_FILES      = 
CDOUT          = ncraft_cd

include ../example.mk

CFLAGS		+= -Isrc -Isrc/pcfx -I. -Wall
