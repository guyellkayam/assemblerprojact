CC          = gcc
CFLAGS      = -ansi -pedantic -Wall
PROG_NAME   = main
BUILD_DIR   = build
OBJ_DIR     = $(BUILD_DIR)/obj
BIN_DIR     = $(BUILD_DIR)/bin

ifdef DEBUG
CFLAGS += -g
endif

.PHONY: clean build_dir all

all: build_dir $(BIN_DIR)/$(PROG_NAME)

$(BIN_DIR)/$(PROG_NAME): $(OBJ_DIR)/main.o $(OBJ_DIR)/preprocessor.o $(OBJ_DIR)/macro_table.o \
                         $(OBJ_DIR)/utils.o $(OBJ_DIR)/analyzer_helper.o $(OBJ_DIR)/analyzer.o \
                         $(OBJ_DIR)/compiled_line.o $(OBJ_DIR)/assembler_helper.o $(OBJ_DIR)/assembler.o \
                         $(OBJ_DIR)/symbol_table.o $(OBJ_DIR)/file_builder.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

build_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)