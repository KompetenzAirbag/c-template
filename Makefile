CXX = gcc
CXX_FLAGS = -MMD -MP -Wall -Wextra

CXX_FLAGS += -g -fsanitize=address -O0
CXX_FLAGS += -fno-omit-frame-pointer

CXX_FLAGS += -DDEBUG
CXX_FLAGS += -DVERBOSE
# CXX_FLAGS += -DNO_WARN
# CXX_FLAGS += -DNO_COLOR

EXEC_BIN = CHANGE_MY_NAME

BUILD_DIR = build

# LD_FLAGS := -l

#####################
#					#
#	   INCLUDES		#
#					#
#####################

# Find dirs with headers
INCLUDEDIRS := $(shell find src -type f -name '*.h' -exec dirname {} \; | sort -u)

# Prefix includes with -I
INCLUDES := $(addprefix -I, $(INCLUDEDIRS))

#####################
#					#
#	  SRC FILES		#
#					#
#####################

# Find all source files except main (so we can have different rules like test)
SRC := $(filter-out src/main.c, $(shell find src -type f -name '*.c'))

MAIN_SRC := src/main.c

#####################
#					#
#	   OBJECTS		#
#					#
#####################

OBJ 	 := $(patsubst src/%.c,  $(BUILD_DIR)/%.o, $(SRC))
MAIN_OBJ := $(patsubst src/%.c,  $(BUILD_DIR)/%.o, $(MAIN_SRC))

#####################
#					#
#	OBJECT RULES	#
#					#
#####################

$(EXEC_BIN): $(OBJ) $(MAIN_OBJ)
	@$(CXX) -o $@ $^ $(LIBS) $(CXX_FLAGS) $(LD_FLAGS)

#####################
#					#
#	  COMPILING		#
#					#
#####################

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

#####################
#					#
#	EXECUTE RULES	#
#					#
#####################

all: $(EXEC_BIN)

run: $(EXEC_BIN)
	./$(EXEC_BIN)

clean:
	rm -rf $(BUILD_DIR) $(EXEC_BIN)

.PHONY: all run clean

# This makes header changes recompile
-include $(OBJ:.o=.d)
-include $(TEST_OBJ:.o=.d)
