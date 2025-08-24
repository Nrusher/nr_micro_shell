CC := gcc
CCFLAG :=
DGBFLAG := -g -fprofile-arcs -ftest-coverage
CCOBJFLAG := $(CCFLAG) -c

BIN_PATH := out
$(shell mkdir -p $(BIN_PATH))

SRC_PATH := ../../src

TARGET_NAME := nr_shell

TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(BIN_PATH)/$(TARGET_NAME)_db

INC :=	-I ../../inc/ \
        -I ./

SRC := 	nr_shell.c \
        $(SRC_PATH)/nr_micro_shell_core.c \
        $(SRC_PATH)/nr_micro_shell_cmds.c

default : all

.PHONY: all
all:
	@rm -f $(TARGET)
	$(CC) $(CCFLAG) $(INC) $(SRC) -o $(TARGET) -l curses

.PHONY: debug
debug: 
	@rm -f $(TARGET_DEBUG)
	$(CC) $(CCFLAG) $(DGBFLAG) $(INC) $(SRC) -o $(TARGET_DEBUG)
.PHONY: clean
clean:
	@echo CLEAN $(TARGET) $(TARGET_DEBUG)
	@rm -f $(TARGET)
	@rm -f $(TARGET_DEBUG)