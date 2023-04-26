TARGET_EXEC_NAME ?= exec

CC = g++
CXX = g++

BUILD_DIR ?= .build
SRC_DIRS ?= src tests
TARGET_EXEC ?= $(BUILD_DIR)/$(TARGET_EXEC_NAME)

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name "*.cpp" -or -name "*.c" ! -name "test.c" -or -name "*.s")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
# PROF_FLAGS := -pg 
OPT_FLAGS := -O3
EXTRA_FLAGS := -std=c++17 # only on Mac runing clang++ (-g)
SCENARIO_FLAG := -DSCENARIO=1

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP $(PROF_FLAGS) $(EXTRA_FLAGS) $(OPT_FLAGS) $(SCENARIO_FLAG) -fPIE -Wno-unused-label 

# -I/tools/Xilinx/Vitis_HLS/2020.2/include 

LDFLAGS := 

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) $(PROF_FLAGS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
.PHONY: run
.PHONY: gprof
.PHONY: graph

clean:
	$(RM) -r $(BUILD_DIR)

run:
	./$(TARGET_EXEC)

gprof:
	gprof $(TARGET_EXEC) gmon.out > gprof.txt | gprof2dot gprof.txt > gprof.dot

graph:
	dot -Tpng gprof.dot -o gprof.png

-include $(DEPS)

MKDIR_P ?= mkdir -p
