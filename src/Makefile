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
SOURCES     = device_manager.cpp syslog_manager.cpp main.cpp
OBJECTS     = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))
OUTPUT      = $(PROJECT_ROOT)/security-tool.exe

# Test programs
TEST_SYSLOG_SRC = $(PROJECT_ROOT)/tools/test_syslog.cpp
TEST_SYSLOG_OUT = $(PROJECT_ROOT)/syslog-test.exe
TEST_SYSLOG_OBJ = $(OBJ_DIR)/test_syslog.o
COMMON_OBJS     = $(OBJ_DIR)/device_manager.o $(OBJ_DIR)/syslog_manager.o

# ============================================================================
# Targets
# ============================================================================

.PHONY: all clean run help test-syslog run-syslog

# Default target - builds everything
all: $(OUTPUT) $(TEST_SYSLOG_OUT)

# Build test programs
test-syslog: $(TEST_SYSLOG_OUT)

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
	rm -f $(OUTPUT) $(TEST_SYSLOG_OUT)
	rm -rf $(OBJ_DIR)/*.o
	@echo "Clean complete!"

# Display help information
help:
	@echo "Available targets:"
	@echo "  all         - Build the program (default)"
	@echo "  run         - Build and run the program"
	@echo "  test-syslog - Build syslog test program"
	@echo "  run-syslog  - Build and run syslog test"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Display this help message"

# ============================================================================
# Test Program Targets
# ============================================================================

# Build syslog test program
$(TEST_SYSLOG_OUT): $(COMMON_OBJS) $(TEST_SYSLOG_OBJ)
	@echo "Linking $(TEST_SYSLOG_OUT)..."
	$(CXX) $(COMMON_OBJS) $(TEST_SYSLOG_OBJ) -o $(TEST_SYSLOG_OUT) $(LIBS) $(LDFLAGS)
	@echo "Syslog test build complete!"

# Compile test_syslog.cpp
$(TEST_SYSLOG_OBJ): $(TEST_SYSLOG_SRC)
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run syslog test
run-syslog: $(TEST_SYSLOG_OUT)
	@echo "Running $(TEST_SYSLOG_OUT)..."
	cd $(PROJECT_ROOT) && ./syslog-test.exe
