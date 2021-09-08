LIBS    = -lm
CC      = gcc
# Flags for Windows/Linux executable (.exe) and Windows object (.o) compiling
FLAGS   = -g -Wall -ggdb
# Flags for Linux object (.o) compiling
SFLAGS  = -g -Wall -ggdb -fPIC
# Flags for Windows Dynamic Link Library (.dll) compiling
DFLAGS  = -g -Wall -ggdb -D BUILD_DLL
# Flags for Linux Shared Object library (.so) compiling
SDFLAGS = -g -Wall -ggdb -shared -D BUILD_DLL
BIN     = ./bin
OBJ     = ./obj
FILE    = main
FILEPFX = $(BIN)/$(FILE).
# The target file type
TARGET  = exe
# The target OS type
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

# Build the executable (.exe), library (.dll) or Unix library (.so)
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
ifeq ($(TOS), OS_WIN)
	@echo === BUILDING DLL ===
	@rm -f "$(BIN)/main.exe"
	$(CC) $(DFLAGS) -D $(TOS) -o $@ $(QOBJS) $(LIBS)
else
	@echo === BUILDING SO ===
	@rm -f "$(BIN)/main.exe"
	$(CC) $(SDFLAGS) -D $(TOS) -o $(basename $@).so $(QOBJS) $(LIBS)
endif
else
	$(error Invalid target type.  Expected "TARGET=exe" or "Target=dll")
endif

# Build the object (.o) files
$(OBJ)/%.o: %.c $(HEADS)
ifeq ($(TARGET)$(TOS), dllOS_UNIX)
	@if [ -n "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
	$(CC) $(SFLAGS) -D $(TOS) -c "$<" -o "$@"
else
	@if [ -n "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
	$(CC) $(FLAGS) -D $(TOS) -c "$<" -o "$@"
endif

$(OBJ) $(BIN):
	@mkdir $@

run: $(BUILD)
	$<

# Delete the folders specified by OBJ and BIN
clean:
	@if [ -d "$(OBJ)" ]; then rm -r $(OBJ); fi; if [ -d "$(BIN)" ]; then rm -r $(BIN); fi
