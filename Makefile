UNAME := $(shell uname)

# Compiler
CC = clang++
CFLAGS = -Wall -Werror -g
LDFLAGS = -L/usr/local/lib 
ifeq ($(UNAME), Darwin)
	LDFLAGS += -L/opt/homebrew/lib
endif
LDFLAGS += -lmpv -lrtmidi -lncurses -llo 

SRC = main.cpp player.cpp tui.cpp midihandler.cpp oschandler.cpp controller.cpp
# SRC += include/inih/cpp/INIReader.cpp
OBJ = $(SRC:.cpp=.o)
# OBJ := $(OBJS:.c=.o)
TARGET = ytmus


INCLUDE = -I/usr/local/include -I./include 
ifeq ($(UNAME), Darwin)
	INCLUDE += -I/opt/homebrew/include
endif
CFLAGS += $(INCLUDE)

# Target
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

# Clean
clean:
	rm -f $(OBJ) .*~ *~
