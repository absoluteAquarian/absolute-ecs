LIBS    = -lm
CC      = gcc
FLAGS   = -g -Wall -ggdb
DFLAGS  = -g -Wall -ggdb -shared -D BUILD_DLL
BIN     = ./bin
OBJ     = ./obj
FILE    = main
FILEPFX = $(BIN)/$(FILE).
TARGET  = exe
BUILD   = $(FILEPFX)$(TARGET)

.PHONY: default all clean

all: $(OBJS) $(BUILD)

SRCS   = $(shell find . -name '*.c' -type f)
HEADS  = $(shell find . -name '*.h' -type f)
OBJS   = $(patsubst %.c, $(OBJ)/%.o, $(SRCS))
QOBJS  = $(wildcard %, "%", $(OBJS))

# This part is supposed to build the executable
$(BUILD): $(OBJ) $(BIN) $(OBJS)
ifeq ($(TARGET), exe)
	@echo === BUILDING EXE ===
	@rm -f "$(BIN)/main.dll"
	$(CC) $(FLAGS) -o $@ $(QOBJS) $(LIBS)
else ifeq ($(TARGET), dll)
	@echo === BUILDING DLL ===
	@rm -f "$(BIN)/main.exe"
	$(CC) $(DFLAGS) -o $@ $(QOBJS) $(LIBS)
else
	$(error Invalid target type.  Expected "TARGET=exe" or "Target=dll")
endif

# This part is supposed to build the object (.o) files
$(OBJ)/%.o: %.c $(HEADS)
	if [ -n "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
	$(CC) $(FLAGS) -c "$<" -o "$@"

$(OBJ) $(BIN):
	@mkdir $@

run: $(BUILD)
	$<

# Delete the folders specified by OBJ and BIN
clean:
	@if [ -d "$(OBJ)" ]; then rm -r $(OBJ); fi; if [ -d "$(BIN)" ]; then rm -r $(BIN); fi
