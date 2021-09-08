LIBS    = -lm
CC      = gcc
FLAGS   = -g -Wall -ggdb
DFLAGS  = -g -Wall -ggdb -shared -D BUILD_DLL
BIN     = ./bin
OBJ     = ./obj
FILE    = main
FILEPFX = $(BIN)/$(FILE).
TARGET  = exe
TOS     = OS_WIN
BUILD   = $(FILEPFX)$(TARGET)

.PHONY: default all clean

all: $(OBJS) $(BUILD)

# Finds all .c files
SRCS   = $(shell find . -name '*.c' -type f)
# Finds all .h files
HEADS  = $(shell find . -name '*.h' -type f)
# First converts all .c file paths to .o file paths,
#  then removes the "./" part of the path to make it look good
OBJS   = $(subst ./,,$(patsubst %.c, $(OBJ)/%.o, $(SRCS)))
# Ensures that paths containing spaces don't cause problems
QOBJS  = $(wildcard %, "%", $(OBJS))

# This part is supposed to build the executable
$(BUILD): $(OBJ) $(BIN) $(OBJS)
ifneq ($(TOS), OS_WIN)
ifneq ($(TOS), OS_UNIX)
$(error Invalid OS specified.  Expected "TOS=OS_WIN" or "TOS=OS_UNIX")
endif
endif
ifeq ($(TARGET), exe)
	@echo === BUILDING EXE ===
	@rm -f "$(BIN)/main.dll"
	$(CC) $(FLAGS) -D $(TOS) -o $@ $(QOBJS) $(LIBS)
else ifeq ($(TARGET), dll)
	@echo === BUILDING DLL ===
	@rm -f "$(BIN)/main.exe"
	$(CC) $(DFLAGS) $(TOS) -o $@ $(QOBJS) $(LIBS)
else
	$(error Invalid target type.  Expected "TARGET=exe" or "Target=dll")
endif

# This part is supposed to build the object (.o) files
$(OBJ)/%.o: %.c $(HEADS)
	@if [ -n "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
	$(CC) $(FLAGS) -D $(TOS) -c "$<" -o "$@"

$(OBJ) $(BIN):
	@mkdir $@

run: $(BUILD)
	$<

# Delete the folders specified by OBJ and BIN
clean:
	@if [ -d "$(OBJ)" ]; then rm -r $(OBJ); fi; if [ -d "$(BIN)" ]; then rm -r $(BIN); fi
