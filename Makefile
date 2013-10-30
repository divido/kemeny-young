# System configuration
CPP   := g++
LDPP  := g++
MKDIR := mkdir -p
RMDIR := rm -rf

DEPFLAGS      :=

CPPFLAGS      :=
DBG_CPPFLAGS  := -O0
OPT_CPPFLAGS  := -O3

LDPPFLAGS     :=
DBG_LDPPFLAGS := -O0
OPT_LDPPFLAGS := -O3

ifeq ($(VERBOSE),)
  Q := @
else
  BLANKLINE := echo
  MKDIR += -v
  RMDIR += -v
endif

# ----------------------------------------
# Useful functions

corresponding-output = $(addprefix $(1)/,$(addsuffix $(2),$(basename $(3))))

# ----------------------------------------
# Directory configuration
SRC_DIR := src
TST_DIR := tests
DEP_DIR := dep
OPT_DIR := opt
DBG_DIR := dbg

OPT_BIN_DIR := $(OPT_DIR)/bin
OPT_LIB_DIR := $(OPT_DIR)/lib
OPT_OBJ_DIR := $(OPT_DIR)/obj
DBG_BIN_DIR := $(DBG_DIR)/bin
DBG_LIB_DIR := $(DBG_DIR)/lib
DBG_OBJ_DIR := $(DBG_DIR)/obj

CLEAN_DIRS := $(DEP_DIR) $(OPT_DIR) $(DBG_DIR)

# ----------------------------------------
# Specific Targets
BIN_MAIN      := kemeny-young
SRCS_MAIN     := $(shell find $(SRC_DIR) -name '*.cpp')
DEPS_MAIN     := $(call corresponding-output,$(DEP_DIR),.d,$(SRCS_MAIN))
DBG_OBJS_MAIN := $(call corresponding-output,$(DBG_OBJ_DIR),.o,$(SRCS_MAIN))
OPT_OBJS_MAIN := $(call corresponding-output,$(OPT_OBJ_DIR),.o,$(SRCS_MAIN))

$(DBG_BIN_DIR)/$(BIN_MAIN): $(DBG_OBJS_MAIN)
$(OPT_BIN_DIR)/$(BIN_MAIN): $(OPT_OBJS_MAIN)

$(DBG_OBJS_MAIN) $(OPT_OBJS_MAIN): CPPFLAGS += -I $(SRC_DIR)
$(DEPS_MAIN): DEPFLAGS += -I $(SRC_DIR)
include $(DEPS_MAIN)

BIN_UTEST      := utest
SRCS_UTEST     := $(shell find $(TST_DIR) -name '*.cpp')
DEPS_UTEST     := $(call corresponding-output,$(DEP_DIR),.d,$(SRCS_UTEST))
DBG_OBJS_UTEST := $(call corresponding-output,$(DBG_OBJ_DIR),.o,$(SRCS_UTEST))
OPT_OBJS_UTEST := $(call corresponding-output,$(OPT_OBJ_DIR),.o,$(SRCS_UTEST))

DBG_EXCLUDE_UTEST := $(call corresponding-output,$(DBG_OBJ_DIR),.o,$(shell find $(SRC_DIR) -name 'kemeny-young.cpp'))
OPT_EXCLUDE_UTEST := $(call corresponding-output,$(OPT_OBJ_DIR),.o,$(shell find $(SRC_DIR) -name 'kemeny-young.cpp'))

$(DBG_BIN_DIR)/$(BIN_UTEST): $(filter-out $(DBG_EXCLUDE_UTEST),$(DBG_OBJS_MAIN)) $(DBG_OBJS_UTEST)
$(OPT_BIN_DIR)/$(BIN_UTEST): $(filter-out $(OPT_EXCLUDE_UTEST),$(OPT_OBJS_MAIN)) $(OPT_OBJS_UTEST)

$(DBG_OBJS_UTEST) $(OPT_OBJS_UTEST): CPPFLAGS += -I $(SRC_DIR) -I $(TST_DIR)
$(DEPS_UTEST): DEPFLAGS += -I $(SRC_DIR) -I $(TST_DIR)
include $(DEPS_UTEST)

ALL_BIN := $(BIN_MAIN) # $(BIN_UTEST)
ALL_LIB :=

# ----------------------------------------
# Basic Targets
.PHONY: all debug optimized test clean
.DEFAULT_GOAL := all

all: debug optimized
debug: $(addprefix $(DBG_BIN_DIR)/,$(ALL_BIN))
optimized: $(addprefix $(OPT_BIN_DIR)/,$(ALL_BIN))
test:
clean:
	@$(RMDIR) $(CLEAN_DIRS)

# ----------------------------------------
# Dependency Generation Rules

$(DEP_DIR)/%.d: %.cpp
	@$(BLANKLINE)
	@echo "Generating Dependencies $<"
	@$(MKDIR) $(dir $@)
	$(Q)$(CPP) -MM -MT $(DBG_OBJ_DIR)/$*.o -MT $(OPT_OBJ_DIR)/$*.o -MP -MG $< $(DEPFLAGS) > $@

# ----------------------------------------
# Compilation Rules

$(DBG_OBJ_DIR)/%.o: COMPILE_TYPE=Debug
$(DBG_OBJ_DIR)/%.o: CPPFLAGS += $(DBG_CPPFLAGS)
$(DBG_OBJ_DIR)/%.o: %.cpp
	@$(BLANKLINE)
	@echo "Compiling [$(COMPILE_TYPE)] $<"
	@$(MKDIR) $(dir $@)
	$(Q)$(CPP) -c $< -o $@ -g $(CPPFLAGS)

$(OPT_OBJ_DIR)/%.o: COMPILE_TYPE=Optimized
$(OPT_OBJ_DIR)/%.o: CPPFLAGS += $(OPT_CPPFLAGS)
$(OPT_OBJ_DIR)/%.o: %.cpp
	@$(BLANKLINE)
	@echo "Compiling [$(COMPILE_TYPE)] $<"
	@$(MKDIR) $(dir $@)
	$(Q)$(CPP) -c $< -o $@ -g $(CPPFLAGS)

# ----------------------------------------
# Link Rules

$(addprefix $(DBG_BIN_DIR)/, $(ALL_BIN)):
	@$(BLANKLINE)
	@echo "Linking [Debug] $@"
	@$(MKDIR) $(dir $@)
	$(Q)$(LDPP) -o $@ -g $(LDPPFLAGS) $(DBG_LDPPFLAGS) $^

$(addprefix $(OPT_BIN_DIR)/, $(ALL_BIN)):
	@$(BLANKLINE)
	@echo "Linking [Optimized] $@"
	@$(MKDIR) $(dir $@)
	$(Q)$(LDPP) -o $@ $(LDPPFLAGS) $(OPT_LDPPFLAGS) $^
