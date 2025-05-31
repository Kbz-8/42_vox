NAME = vox

SRCS += $(wildcard $(addsuffix /*.cpp, ./Application))

BIN_DIR = Bin
OBJ_DIR = Objects
RES_DIR = Resources

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

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

$(NAME): $(OBJ_DIR) $(BIN_DIR) engine $(OBJS)
	@printf "$(COLOR)($(_BOLD)100%%$(_RESET)$(COLOR)) $(_RESET)Compiling $(_BOLD)Resources/Fragment.nzsl$(_RESET)\n"
	@$(NZSLC) --compile=spv Resources/Fragment.nzsl -o Resources/ --optimize --module="ScopEngine/Assets/Shaders/Modules"
	@printf "$(COLOR)($(_BOLD)100%%$(_RESET)$(COLOR)) $(_RESET)Compiling $(_BOLD)Resources/Screen.nzsl$(_RESET)\n"
	@$(NZSLC) --compile=spv Resources/PostProcess.nzsl -o Resources/ --optimize --module="ScopEngine/Assets/Shaders/Modules"
	@printf "Linking $(_BOLD)$(NAME)$(_RESET)\n"
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS)
	@printf "$(_BOLD)$(NAME)$(_RESET) compiled $(COLOR)$(_BOLD)successfully$(_RESET)\n"

engine:
	@make -j$(JOBS) -C ScopEngine DEBUG=$(DEBUG) --no-print-directory

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

run:
	@$(BIN_DIR)/$(NAME) $(RES_DIR)/42.obj

re-shaders:
	@make -C ScopEngine re-shaders

clean:
	@$(RM) $(OBJ_DIR)
	@make -C ScopEngine clean

fclean: clean
	@$(RM) $(BIN_DIR)
	@make -C ScopEngine fclean

re: fclean all

.PHONY: all run clean fclean re re-shaders
