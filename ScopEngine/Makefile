SRCS =  $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Core))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Platform))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics/Cameras))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics/Loaders))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Memory))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Vulkan))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Pipelines))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/RenderPasses))

SHADER_SRCS = $(wildcard $(addsuffix /*.nzsl, ./Assets/Shaders))

BIN_DIR = Bin
OBJ_DIR = Objects
SHADER_DIR = Assets/Shaders/Build
SHADER_MODULE_DIR = Assets/Shaders/Modules

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
SPVS = $(addprefix $(SHADER_DIR)/, $(SHADER_SRCS:.nzsl=.spv))

CXX = clang++
CXXFLAGS = -std=c++20 -I Runtime/Includes -I Runtime/Sources -I ThirdParty/ -I ThirdParty/KVF -D KVF_IMPL_VK_NO_PROTOTYPES -D VK_NO_PROTOTYPES -fPIC

AR = ar rc

SH = sh -c

NZSLC = ./Assets/Vendors/nzslc.x86_64

LDFLAGS = -lSDL2

SHARED ?= false
DEBUG ?= false

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
	CXXFLAGS += -g3 -D DEBUG -D IMGUI_IMPL_VULKAN_NO_PROTOTYPES -I ThirdParty/imgui
	SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Debug))
	SRCS += $(wildcard $(addsuffix /*.cpp, ./ThirdParty/imgui))
	SRCS += $(wildcard $(addsuffix /*.cpp, ./ThirdParty/imgui/backends))
	MODE := $(_RESET)$(_PURPLE)$(_BOLD)Debug$(_RESET)$(_PURPLE)
	COLOR := $(_PURPLE)
else
	MODE := $(_RESET)$(_GREEN)$(_BOLD)Release$(_RESET)$(_GREEN)
	COLOR := $(_GREEN)
endif

ifeq ($(SHARED), true)
	NAME = engine.so
else
	NAME = engine.a
endif

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

all: _printbuildinfos $(NAME)

$(NAME): $(OBJ_DIR) $(BIN_DIR) shaders $(OBJS)
	@printf "Linking $(_BOLD)$(NAME)$(_RESET)\n"
	@$(CXX) -shared -o $(NAME) $(OBJS) $(LDFLAGS)
ifeq ($(SHARED), true)
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS) --shared
else
	@$(AR) $(BIN_DIR)/$(NAME) $(OBJS)
endif
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

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(SHADER_DIR):
	@mkdir -p $(SHADER_DIR)

shaders: $(SHADER_DIR) $(SPVS)

_printbuildinfos:
	@printf "$(_PURPLE)$(_BOLD)ScopEngine $(_RESET)Compiling in $(_BOLD)$(MODE)$(_RESET) mode on $(_BOLD)$(OS)$(_RESET) | Using $(_BOLD)$(CXX) ($(shell $(CXX) --version | head -n 1))$(_RESET), flags: $(_BOLD)$(_ENABLEDFLAGS)$(_RESET)\n"

debug:
	@$(MAKE) all DEBUG=true -j$(shell nproc)

dependencies:
	@$(SH) Script/FetchDependencies.sh

clean-shaders:
	@$(RM) $(SHADER_DIR)

re-shaders: clean-shaders shaders

clean: clean-shaders
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(BIN_DIR)

re: fclean all

.PHONY: all clean fclean re dependencies shaders clean-shaders re-shaders
