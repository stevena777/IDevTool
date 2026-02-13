# ============================================================================
# Makefile for iOS Device Security Tool
# ============================================================================

# Compiler and flags
CXX         = g++
CXXFLAGS    = -Wall -Wextra -O2 -std=c++11
LDFLAGS     =

# Directories
PROJECT_ROOT = ..
MSYS_PREFIX  = /c/msys64/ucrt64
INCLUDE_DIR  = $(MSYS_PREFIX)/include
LIB_DIR      = $(MSYS_PREFIX)/lib
PROJECT_INC  = $(PROJECT_ROOT)/include
OBJ_DIR      = $(PROJECT_ROOT)/obj

# Include paths
INCLUDES     = -I$(PROJECT_INC) -I$(INCLUDE_DIR)

# Libraries
LIBS        = -L$(LIB_DIR) \
              -limobiledevice-1.0 \
              -limobiledevice-glue-1.0 \
              -lusbmuxd-2.0 \
              -lplist-2.0 \
              -lplist++-2.0

# Source files and output
SOURCES     = device_manager.cpp main.cpp
OBJECTS     = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))
OUTPUT      = $(PROJECT_ROOT)/security-tool.exe

# ============================================================================
# Targets
# ============================================================================

.PHONY: all clean run help

# Default target
all: $(OUTPUT)

# Build the executable
$(OUTPUT): $(OBJECTS)
	@echo "Linking $(OUTPUT)..."
	$(CXX) $(OBJECTS) -o $(OUTPUT) $(LIBS) $(LDFLAGS)
	@echo "Build complete!"

# Compile C++ source files to object files
$(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the program
run: $(OUTPUT)
	@echo "Running $(OUTPUT)..."
	cd $(PROJECT_ROOT) && ./security-tool.exe

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OUTPUT)
	rm -rf $(OBJ_DIR)/*.o
	@echo "Clean complete!"

# Display help information
help:
	@echo "Available targets:"
	@echo "  all    - Build the program (default)"
	@echo "  run    - Build and run the program"
	@echo "  clean  - Remove build artifacts"
	@echo "  help   - Display this help message"
