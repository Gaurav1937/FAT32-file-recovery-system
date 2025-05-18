# Makefile for FAT32 recovery tool

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
LDFLAGS = -lwchar

SRCS    = main.c fat32_utils.c
OBJS    = $(SRCS:.c=.o)
TARGET  = recover

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c fat32_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean


# Makefile Breakdown

# makefile
# CC      = gcc
# CC is the variable holding the compiler you want to use. Here it's gcc.


# makefile
# CFLAGS  = -Wall -Wextra -std=c11

# CFLAGS contains compiler options:
#		 -Wall enables all common warnings
# 		-Wextra enables extra warnings.
# 		-std=c11 tells the compiler to use the C11 standard.



# makefile
# LDFLAGS = -lwchar
# LDFLAGS contains linker flags; here, it links the wchar library needed for wide character functions.



# makefile
# SRCS    = main.c fat32_utils.c
# SRCS lists all the source files to compile.



# makefile
# OBJS    = $(SRCS:.c=.o)
# OBJS converts the .c filenames in SRCS into .o object files.
# So main.c becomes main.o, fat32_utils.c becomes fat32_utils.o.



# makefile
# TARGET  = recover
# TARGET is the name of the final executable your program will produce.



# makefile
# all: $(TARGET)
# The default target (what happens if you just run make) depends on $(TARGET) (the executable).

# This means make will try to build the executable recover.



# makefile
# $(TARGET): $(OBJS)
# 	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# This rule tells how to build the executable recover from object files.
# $@ is an automatic variable meaning "the target" (here, recover).
# It compiles the object files and links them into the final executable.
# $(LDFLAGS) includes any linker flags (like linking -lwchar).



# makefile
# %.o: %.c fat32_utils.h
# 	$(CC) $(CFLAGS) -c $< -o $@
# This is a pattern rule to compile any .c file to .o.

# $< is the first prerequisite (the .c file), and $@ is the target (the .o file).

# It also depends on fat32_utils.h (header), so if the header changes, the object files are rebuilt.

# The -c flag tells gcc to compile only (no linking).



# makefile
# clean:
# 	rm -f $(OBJS) $(TARGET)

# The clean target removes all .o files and the executable recover.
# Use it to clean up your build directory.



# makefile
# .PHONY: all clean
# Declares all and clean as "phony" targets — they don’t correspond to actual files.

# This prevents confusion if files named all or clean exist.