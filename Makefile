PRGNAME     = mccraft.elf
CC			= gcc

SRCDIR		= ./src ./src/sdl
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_S		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.s))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_S		= $(notdir $(patsubst %.s, %.o, $(SRC_CP)))
OBJS		= $(OBJ_C) $(OBJ_S)

CFLAGS		= -O0 -g3 -D_16BIT
CFLAGS		+= -Isrc -Idata -Isrc/sdl

LDFLAGS     = -nodefaultlibs -lc -lgcc -lm -lSDL -lSDL_image -lasound

# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(CC) $(CFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(OBJ_S) : %.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o
