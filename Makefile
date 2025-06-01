NAME = vox

SRCS += $(wildcard $(addsuffix /*.cpp, ./Application))

SHADER_SRCS = $(wildcard $(addsuffix /*.nzsl, ./Resources/Shaders))

BIN_DIR = Bin
OBJ_DIR = Objects
RES_DIR = Resources
SHADER_DIR = Resources/Shaders/Build
SHADER_MODULE_DIR = ScopEngine/Assets/Shaders/Modules

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
SPVS = $(addprefix $(SHADER_DIR)/, $(SHADER_SRCS:.nzsl=.spv))

CXX = clang++
CXXFLAGS = -std=c++20 -I ScopEngine/Runtime/Includes -I Application -I ScopEngine/ThirdParty/KVF -I ScopEngine/ThirdParty -D KVF_IMPL_VK_NO_PROTOTYPES

LDFLAGS = -lSDL2 ScopEngine/Bin/engine.a

DEBUG ?= false
MODE = "release"

NZSLC = ./ScopEngine/Assets/Vendors/nzslc.x86_64

TPUT = tput -T xterm-256color
_RESET := $(shell $(TPUT) sgr0)
_BOLD := $(shell $(TPUT) bold)
_ITALIC := $(shell $(TPUT) sitm)
_UNDER := $(shell $(TPUT) smul)
_GREEN := $(shell $(TPUT) setaf 2)
_YELLOW := $(shell $(TPUT) setaf 3)
_RED := $(shell $(TPUT) setaf 1)
_GRAY := $(shell $(TPUT) setaf 8)
_PURPLE := $(shell $(TPUT) setaf 5)

ifeq ($(DEBUG), true)
	CXXFLAGS += -g -D DEBUG
	MODE := $(_RESET)$(_PURPLE)$(_BOLD)Debug$(_RESET)$(_PURPLE)
	COLOR := $(_PURPLE)
else
	MODE := $(_RESET)$(_GREEN)$(_BOLD)Release$(_RESET)$(_GREEN)
	COLOR := $(_GREEN)
endif

JOBS = $(patsubst -j%,%,$(filter -j%,$(MAKEFLAGS)))

RM = rm -rf

OBJS_TOTAL = $(words $(OBJS))
N_OBJS := $(shell find $(OBJ_DIR) -type f -name '*.o' 2>/dev/null | wc -l)
OBJS_TOTAL := $(shell echo $$(( $(OBJS_TOTAL) - $(N_OBJS) )))
ifeq ($(OBJS_TOTAL), 0) # To avoid division per 0
	OBJS_TOTAL := 1
endif
CURR_OBJ = 0

$(OBJ_DIR)/%.o: %.cpp
	@$(eval CURR_OBJ=$(shell echo $$(( $(CURR_OBJ) + 1 ))))
	@$(eval PERCENT=$(shell echo $$(( $(CURR_OBJ) * 100 / $(OBJS_TOTAL) ))))
	@printf "$(COLOR)($(_BOLD)%3s%%$(_RESET)$(COLOR)) $(_RESET)Compiling $(_BOLD)$<$(_RESET)\n" "$(PERCENT)"
	@$(CXX) $(CXXFLAGS) $(COPTS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ_DIR) $(BIN_DIR) shaders engine $(OBJS)
	@printf "Linking $(_BOLD)$(NAME)$(_RESET)\n"
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS)
	@printf "$(_BOLD)$(NAME)$(_RESET) compiled $(COLOR)$(_BOLD)successfully$(_RESET)\n"

SPVS_TOTAL = $(words $(SPVS))
N_SPVS := $(shell find $(SHADERS_DIR) -type f -name '*.spv.h' 2>/dev/null | wc -l)
SPVS_TOTAL := $(shell echo $$(( $(SPVS_TOTAL) - $(N_SPVS) )))
ifeq ($(SPVS_TOTAL), 0) # Same
	SPVS_TOTAL := 1
endif
CURR_SPV = 0

$(SHADER_DIR)/%.spv: %.nzsl
	@$(eval CURR_SPV=$(shell echo $$(( $(CURR_SPV) + 1 ))))
	@$(eval PERCENT=$(shell echo $$(( $(CURR_SPV) * 100 / $(SPVS_TOTAL) ))))
	@printf "$(COLOR)($(_BOLD)%3s%%$(_RESET)$(COLOR)) $(_RESET)Compiling $(_BOLD)$<$(_RESET)\n" "$(PERCENT)"
	@$(NZSLC) --compile=spv $< -o $(SHADER_DIR) --optimize --module=$(SHADER_MODULE_DIR)

engine:
	@make -j$(JOBS) -C ScopEngine DEBUG=$(DEBUG) --no-print-directory

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(SHADER_DIR):
	@mkdir -p $(SHADER_DIR)

shaders: $(SHADER_DIR) $(SPVS)

run:
	@$(BIN_DIR)/$(NAME) $(RES_DIR)/42.obj

clean-shaders:
	@$(RM) $(SHADER_DIR)

re-shaders: clean-shaders shaders
	@make -C ScopEngine re-shaders

clean:
	@$(RM) $(OBJ_DIR)
	@make -C ScopEngine clean

fclean: clean
	@$(RM) $(BIN_DIR)
	@make -C ScopEngine fclean

re: fclean all

.PHONY: all run clean fclean re clean-shaders re-shaders
