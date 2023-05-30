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
ifeq ($(UNAME_S),Linux) 
	PROF_FLAGS := -pg -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline -fno-omit-frame-pointer
endif
C_XX_OPT_FLAGS := -O3 -ftree-vectorize -fopt-info-vec
EXTRA_FLAGS := -std=c++17 # only on Mac runing clang++
SCENARIO := 1
SCENARIO_FLAG := -DSCENARIO=$(SCENARIO)
SGN_ALPHA_OPT := 0
POW_OPT := 0
ISNAN_ISINF := 1
CODE_OPT_FLAGS := -D_POW_OPT_=$(POW_OPT) -D_SGN_ALPHA_OPT_=$(SGN_ALPHA_OPT) -D_ISNAN_ISINF_=$(ISNAN_ISINF)

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP $(PROF_FLAGS) $(EXTRA_FLAGS) $(C_XX_OPT_FLAGS) $(SCENARIO_FLAG) $(CODE_OPT_FLAGS) -fPIE -Wno-unused-label 

# -I/tools/Xilinx/Vitis_HLS/2020.2/include 

LDFLAGS := 

ifeq ($(UNAME_S),Linux) 
all: clean $(TARGET_EXEC)_$(SCENARIO) run gprof graph
else 
all: clean $(TARGET_EXEC)_$(SCENARIO) run
endif

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
	echo $(PROF_FLAGS)
	$(RM) -r $(BUILD_DIR)

run:
	./$(TARGET_EXEC)_$(SCENARIO)

gprof: ./$(TARGET_EXEC)_$(SCENARIO)
	gprof $(TARGET_EXEC)_$(SCENARIO) gmon.out > $(GPROF_DIR)/gprof_$(SCENARIO).txt | gprof2dot $(GPROF_DIR)/gprof_$(SCENARIO).txt > $(GPROF_DIR)/gprof_$(SCENARIO).dot

graph: 
	dot -Tpng $(GPROF_DIR)/gprof_$(SCENARIO).dot -o $(GPROF_DIR)/gprof_$(SCENARIO).png

-include $(DEPS)

MKDIR_P ?= mkdir -p
