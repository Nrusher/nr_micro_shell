CC := gcc
CCFLAG := 
DGBFLAG := -g
CCOBJFLAG := $(CCFLAG) -c

BIN_PATH := out
SRC_PATH := ../src

TARGET_NAME := nr_micro_shell

TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(BIN_PATH)/$(TARGET_NAME)_db

INC :=	../../inc/

SRC := 	simulator.c \
		../nr_micro_shell_commands.c \
		../../src/ansi_port.c \
		../../src/ansi.c \
		../../src/nr_micro_shell.c

default : all

.PHONY: all
all: 
	@rm -f $(TARGET)
	$(CC) $(CCFLAG) -I $(INC) -D NR_MICRO_SHELL_SIMULATOR $(SRC) -o $(TARGET)

.PHONY: debug
debug: 
	@rm -f $(TARGET_DEBUG)
	$(CC) $(CCFLAG) $(DGBFLAG) -I $(INC) -D NR_MICRO_SHELL_SIMULATOR $(SRC) -o $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(TARGET) $(TARGET_DEBUG)
	@rm -f $(TARGET)
	@rm -f $(TARGET_DEBUG)

	


