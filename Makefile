TARGET_EXEC ?= StoneEng

CC = clang

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS = -std=gnu17 -Wall -pg -Wextra -pedantic -O3 

LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl

ifndef asan
	ASAN = 
else 
	ASAN = -fsanitize=address
endif


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -g -o $@ $(LDFLAGS) $(ASAN) 

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(ASAN)

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)



-include $(DEPS)

MKDIR_P ?= mkdir -p


