TARGET_EXEC_NAME ?= exec

CC = g++
CXX = g++

BUILD_DIR ?= .build
SRC_DIRS ?= src tests
GPROF_DIR ?= gprof
TARGET_EXEC ?= $(BUILD_DIR)/$(TARGET_EXEC_NAME)
# Get the OS name
UNAME_S := $(shell uname -s)

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name "*.cpp" -or -name "*.c" ! -name "test.c" -or -name "*.s")
SRCS := $(filter-out tests/main.cpp, $(SRCS)) # removing the old main
SRCS := $(filter-out tests/Tester.cpp, $(SRCS)) # removing the old main
SRCS := $(filter-out tests/JsonExporter.cpp, $(SRCS)) # removing the old main
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
# Gprof flags
# -fprile-arcs: generate extra code to write profile information suitable for the analysis program gprof
# -ftest-coverage: compile with coverage information for each object file
# -fno-inline: do not inline functions
# -fno-inline-small-functions: do not inline functions that are both small and marked inline (removed)
# -fno-default-inline: do not make any functions inline by default (removed)
# -fno-omit-frame-pointer: do not omit frame pointers for leaf functions (functions that do not call other functions)
# -fopt-info-vec: print information about vectorization 
ifeq ($(UNAME_S),Linux) 
	PROF_FLAGS := -pg 
	PROF_FLAGS := $(PROF_FLAGS) -fprofile-arcs -ftest-coverage -fno-inline -fno-omit-frame-pointer
	PROF_FLAGS := $(PROF_FLAGS) -fopt-info-vec
endif
C_XX_OPT_FLAGS :=  # -O3 -march=native -ftree-vectorize
EXTRA_FLAGS := -std=c++17 # only on Mac runing clang++
SCENARIO := 1
SCENARIO_FLAG := DSCENARIO=$(SCENARIO)
SGN_ALPHA_OPT := 0
SGN_ALPHA_OPT_FLAG := D_SGN_ALPHA_OPT_=$(SGN_ALPHA_OPT)
POW_OPT := 0
POW_OPT_FLAG := D_POW_OPT_=$(POW_OPT)
ISNAN_ISINF := 1
ISNAN_ISINF_FLAG :=D_ISNAN_ISINF_=$(ISNAN_ISINF)

CODE_OPT_FLAGS := -$(POW_OPT_FLAG) -$(SGN_ALPHA_OPT_FLAG) -$(ISNAN_ISINF_FLAG)

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP $(PROF_FLAGS) $(EXTRA_FLAGS) $(C_XX_OPT_FLAGS) -$(SCENARIO_FLAG) $(CODE_OPT_FLAGS) -fPIE -Wno-unused-label 

# -I/tools/Xilinx/Vitis_HLS/2020.2/include 

TARGET_EXTENSION_NAME := $(SCENARIO_FLAG)_$(SGN_ALPHA_OPT_FLAG)_$(POW_OPT_FLAG)_$(ISNAN_ISINF_FLAG)
TARGET_NAME := $(TARGET_EXEC)_$(TARGET_EXTENSION_NAME)

LDFLAGS := 

ifeq ($(UNAME_S),Linux) 
all: clean $(TARGET_NAME) run gprof graph
else 
all: clean $(TARGET_NAME) hyperfine
endif

$(TARGET_NAME): $(OBJS)
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
.PHONY: clean-gprof
.PHONY: advisor
.PHONY: hyperfine

clean:
	$(RM) -r $(BUILD_DIR)

run:
	./$(TARGET_NAME)

gprof: $(TARGET_NAME)
	gprof $(TARGET_NAME) gmon.out > $(GPROF_DIR)/gprof_$(TARGET_EXTENSION_NAME).txt | gprof2dot $(GPROF_DIR)/gprof_$(TARGET_EXTENSION_NAME).txt > $(GPROF_DIR)/gprof_$(TARGET_EXTENSION_NAME).dot

graph: 
	dot -Tpng $(GPROF_DIR)/gprof_$(TARGET_EXTENSION_NAME).dot -o $(GPROF_DIR)/gprof_$(TARGET_EXTENSION_NAME).png

clean-gprof: 
	$(RM) -r $(GPROF_DIR)/*

advisor:
	# TODO
	@echo "To be defined"

hyperfine:
	hyperfine -N --warmup 5 --runs 100 './$(TARGET_NAME)'

-include $(DEPS)

MKDIR_P ?= mkdir -p
