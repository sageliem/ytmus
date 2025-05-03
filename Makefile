# Compiler
CC = clang++
CFLAGS = -Wall -g
LDFLAGS = -L/usr/local/lib -lmpv -lrtmidi -lncurses

SRC = main.cpp player.cpp tui.cpp midictl.cpp controller.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = ytcc2

# Target
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -I/usr/local/include -c $<

# Clean
clean:
	rm -f $(OBJ) .*~ *~
