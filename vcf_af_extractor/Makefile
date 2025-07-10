# Makefile for statically compiling the vcf_af_extractor program.
#
# This Makefile links against a custom, statically-built htslib library
# to create a fully standalone executable.

# --- Configuration ---
# IMPORTANT: Update this path to your htslib installation directory.
# For example: HTSLIB_DIR = /home/user/htslib
HTSLIB_DIR = /path/to/your/htslib

# Compiler
CC = gcc

# --- Compiler and Linker Flags ---
CFLAGS = -Wall -O3 -I$(HTSLIB_DIR)/include

# STATIC_LIBS: The path to the static htslib library and its dependencies.

STATIC_LIBS = $(HTSLIB_DIR)/lib/libhts.a -lpthread -lz -lbz2 -llzma -lcurl -lcrypto -ldeflate -lm

# --- File and Target Definitions ---
# Source file and target executable
SRC = vcf_af_extractor.c
TARGET = vcf_af_extractor

# --- Build Rules ---
# Default target: builds the program.
all: $(TARGET)

# Rule to build the standalone executable using static linking.
# It uses the STATIC_LIBS variable to ensure everything is bundled.
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(STATIC_LIBS)
	@echo "---"
	@echo "Compilation successful! Standalone executable created: $(TARGET)"
	@echo "You can verify it has no dynamic htslib dependency by running: ldd $(TARGET)"
	@echo "---"

# Clean target: removes the compiled executable.
clean:
	rm -f $(TARGET)
	@echo "Cleaned up compiled files."

# Declare phony targets (targets that are not files).
.PHONY: all clean

