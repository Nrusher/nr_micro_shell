CC := gcc
CCFLAG :=
DGBFLAG := -g -fprofile-arcs -ftest-coverage --coverage -lgcov -D NR_DEBUG_BIN -fPIC -O0
CCOBJFLAG := $(CCFLAG) -c

BIN_PATH := out
SRC_PATH := ../../src

TARGET_NAME := nr_shell

TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(BIN_PATH)/$(TARGET_NAME)_db

INC :=	-I ../../inc/ \
        -I ./

SRC := 	simulator.c \
	    nr_micro_shell_port.c \
        $(SRC_PATH)/nr_micro_shell_core.c \
        $(SRC_PATH)/nr_micro_shell_cmds.c

default : all

.PHONY: all
all:
	@rm -f $(TARGET)
	$(CC) $(CCFLAG) $(INC) -D NR_MICRO_SHELL_SIMULATOR $(SRC) -o $(TARGET) -l curses

.PHONY: debug
debug:
	@rm -f $(TARGET_DEBUG)
	$(CC) $(CCFLAG) $(DGBFLAG) $(INC) -D NR_MICRO_SHELL_SIMULATOR $(SRC) -o $(TARGET_DEBUG) -l curses

.PHONY: clean
clean:
	@echo CLEAN $(TARGET) $(TARGET_DEBUG)
	@rm -f $(TARGET)
	@rm -f $(TARGET_DEBUG)