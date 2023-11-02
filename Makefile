CC = gcc
CFLAGS = -Wall -std=c99 -O2 -Isrc -Ideps/raylib/src
LDFLAGS = -Ldeps/raylib/src -lraylib -lm
SRC_DIR = src
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/background.c $(SRC_DIR)/player.c $(SRC_DIR)/enemies.c $(SRC_DIR)/particles.c $(SRC_DIR)/title.c
TARGET = RainbowBullet
RAYLIB_DIR = deps/raylib/src
PLATFORM ?= PLATFORM_DESKTOP
CFLAGS += -D$(PLATFORM)

ifeq ($(PLATFORM), PLATFORM_WEB)
	CC = emcc
	CFLAGS += -s WASM=1 -s ASYNCIFY=1 -s USE_GLFW=3 -s EXPORTED_FUNCTIONS=_main,_Pause,_Resume -s EXPORTED_RUNTIME_METHODS=ccall
	LDFLAGS += --shell-file shell.html
endif

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
	rm -f $(TARGET).wasm

.PHONY: all clean

setup:
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=$(PLATFORM)

setupClean:
	$(MAKE) -C $(RAYLIB_DIR) clean
