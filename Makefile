CC          = gcc
CFLAGS      = -ansi -pedantic -Wall -std=c89
PROG_NAME   = assembler
BUILD_DIR   = build
OBJ_DIR     = $(BUILD_DIR)/obj
BIN_DIR     = $(BUILD_DIR)/bin

ifdef DEBUG
CFLAGS += -g
endif

SRCS = main.c \
       preprocessor/preprocessor.c \
       preprocessor/macro_table.c \
       helpers/utils.c \
       helpers/analyzer_helper.c \
       analyzer/analyzer.c \
       data_structures/compiled_line.c \
       helpers/assembler_helper.c \
       assembler/assembler.c \
       data_structures/symbol_table.c \
       file_builder/file_builder.c

OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

.PHONY: all clean build_dir

all: build_dir $(BIN_DIR)/$(PROG_NAME)

$(BIN_DIR)/$(PROG_NAME): $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: preprocessor/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: helpers/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: analyzer/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: data_structures/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: assembler/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: file_builder/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

build_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

print_objs:
	@echo "Object files: $(OBJS)"