TARGET_EXEC_NAME ?= exec

CC = g++
CXX = g++

BUILD_DIR ?= .build
SRC_DIRS ?= src tests
GPROF_DIR ?= gprof
TARGET_EXEC ?= $(BUILD_DIR)/$(TARGET_EXEC_NAME)

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name "*.cpp" -or -name "*.c" ! -name "test.c" -or -name "*.s")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
# Gprof flags
# -fprile-arcs: generate extra code to write profile information suitable for the analysis program gprof
# -ftest-coverage: compile with coverage information for each object file
# -fno-inline: do not inline functions
# -fno-inline-small-functions: do not inline functions that are both small and marked inline
# -fno-default-inline: do not make any functions inline by default
PROF_FLAGS := -pg -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline 
OPT_FLAGS := -O3
EXTRA_FLAGS := -std=c++17 # only on Mac runing clang++ (-g)
SCENARIO := 1
SCENARIO_FLAG := -DSCENARIO=$(SCENARIO)
MINMAX_NORMALIZE_OPT := 0
MINMAX_OPT := 0
UPDATE_OPT := 0
EVALUALTE_SPLIT_OPT := 0
UPDATE_QUANTILES_OPT := 0
OPTMIZATION_FLAGS := -D_MINMAX_NORMALIZE_OPT_=$(MINMAX_NORMALIZE_OPT) -D_MINMAX_OPT_=$(MINMAX_OPT) -D_UPDATE_OPT_=$(UPDATE_OPT) -D_EVALUALTE_SPLIT_OPT_=$(EVALUALTE_SPLIT_OPT) -D_UPDATE_QUANTILES_OPT_=$(UPDATE_QUANTILES_OPT)

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP $(PROF_FLAGS) $(EXTRA_FLAGS) $(OPT_FLAGS) $(SCENARIO_FLAG) $(OPTMIZATION_FLAGS) -fPIE -Wno-unused-label 

# -I/tools/Xilinx/Vitis_HLS/2020.2/include 

LDFLAGS := 

all: clean $(TARGET_EXEC)_$(SCENARIO) run gprof graph

$(TARGET_EXEC)_$(SCENARIO): $(OBJS)
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
	$(RM) -r $(BUILD_DIR) $(GPROF_DIR)/*

run:
	./$(TARGET_EXEC)_$(SCENARIO)

gprof: run
	gprof $(TARGET_EXEC)_$(SCENARIO) gmon.out > $(GPROF_DIR)/gprof_$(SCENARIO).txt | gprof2dot $(GPROF_DIR)/gprof_$(SCENARIO).txt > $(GPROF_DIR)/gprof_$(SCENARIO).dot

graph: 
	dot -Tpng $(GPROF_DIR)/gprof_$(SCENARIO).dot -o $(GPROF_DIR)/gprof_$(SCENARIO).png

-include $(DEPS)

MKDIR_P ?= mkdir -p
