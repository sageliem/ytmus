UNAME := $(shell uname)

# Compiler
CC = clang++
CFLAGS = -Wall -Werror -g
LDFLAGS = -L/usr/local/lib -lmpv -lrtmidi -lncurses -llo


SRC = main.cpp player.cpp tui.cpp midihandler.cpp oschandler.cpp controller.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = ytmus


INCLUDE = -I/usr/local/include 
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
